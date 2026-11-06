#include "Playing.h"
#include "StateHelper.h"

void GameState::Playing::update(GameFSM::FullControl& control)
{
    GameContext& context = control.context();
    if (!context.world || !context.player) return;

    if (IsKeyPressed(KEY_ESCAPE)) {
        control.changeTo<GameState::Paused>();
        return;
    }

    Map& map = context.world->getMap();

    const Vector2 mouseWorld = context.world->getCamera().screenToWorld2D(GetMousePosition());
    context.player->setAimTarget(mouseWorld);

    const Vector2 before = context.player->getPosition();
    context.player->update(context.deltaTime);
    StateHelper::keepOutOfObstacles(map, *context.player, before);

    // E: use a lever, key, switch, rune or tablet or a locked door. Otherwise free the prisoner.
    if (IsKeyPressed(KEY_E)) {
        const Vector2 p = context.player->getPosition();
        if (!map.interact(p) && map.freePrisoner(p)) {
            context.allies.push_back(StateHelper::makeTrooperAlly(map.getPrisonerPosition()));
        }
    }

    //if (IsKeyPressed(KEY_Q)) {
    //    context.player->getCharacter().activateAbility();
    //}
    if (IsKeyPressed(KEY_Q)) {
        Character& character = context.player->getCharacter();

        if (character.activateAbility() && character.getAvatar().summonsClone()) {
            const Vector2 p = context.player->getPosition();
            const Vector2 aim = context.player->getAimDirection();

            // Appears just in front of the player, so it is already facing the fight.
            context.allies.push_back(StateHelper::makeShadowClone(
                Vector2{ p.x + aim.x * 28.0f, p.y + aim.y * 28.0f },
                character.getAvatar().getAbilityDuration()));
        }
    }

    for (UniquePtr<Ally>& ally : context.allies) {
        const Vector2 allyBefore = ally->getPosition();
        ally->setLeader(context.player->getPosition());
        ally->update(context.deltaTime);
        StateHelper::keepOutOfObstacles(map, *ally, allyBefore);
    }

    // Enemy targets: index 0 is the player, index 1+i is allies[i].
    std::vector<Target> targets;
    targets.push_back({ context.player->getHitPosition(), context.player->getHitRadius(), !context.player->isDead() });
    for (const UniquePtr<Ally>& ally : context.allies) {
        targets.push_back({ ally->getHitPosition(), ally->getHitRadius(), !ally->isDead() });
    }

    context.world->update(context.deltaTime, context.player->getPosition(), targets);

    for (const Item& item :
        context.world->collectItems(context.player->getPosition()))
    {
        const Attachment& attachment = item.attachment;

        std::visit(
            [&](const auto& value)
            {
                using T = std::decay_t<decltype(value)>;

                if constexpr (std::is_same_v<T, GunAttachment>)
                {
                    ++context.attachmentStock[indexOf(value)];
                }
                else if constexpr (std::is_same_v<T, SwordAttachment>)
                {
                    ++context.attachmentStock[indexOf(value)];
                }
            },
            attachment.attachment
        );

        context.notice = "Found: "
            + Weapon::getAttachmentName(attachment)
            + "  (install it in the Armory)";

        context.noticeTimer = 3.0f;
    }


    context.noticeTimer = std::max(0.0f, context.noticeTimer - context.deltaTime);

    // Coins for every enemy defeated since the last frame.
    const int kills = context.world->getStage().getKills();
    context.player->getCoins() = context.player->getCoins() + (kills - context.lastKills) * StateHelper::CoinPerKill;
    context.lastKills = kills;

    if (targets[0].damage > 0.0f) context.player->takeDamage(targets[0].damage);
    for (size_t i = 0; i < context.allies.size(); ++i) {
        if (targets[i + 1].damage > 0.0f) context.allies[i]->takeDamage(targets[i + 1].damage);
    }

    if (context.player->isDead()) {
        control.changeTo<GameState::GameOver>();
        return;
    }

    // Enough enemies defeated: the stage is complete and the next one unlocks.
    if (context.world->getStage().isComplete()) {
        context.unlockedStages = std::max(context.unlockedStages, std::min(StageConfig::Count, context.selectedStage + 1));
        control.changeTo<GameState::StageComplete>();
        return;
    }

    std::erase_if(context.allies, [](const UniquePtr<Ally>& ally) { return ally->isDead(); });

    std::vector<Enemy*> enemies;
    context.world->collectTargets(enemies);
    context.player->updateWeapons(context.deltaTime, enemies);

    for (UniquePtr<Ally>& ally : context.allies) {
        ally->updateCombat(context.deltaTime, enemies);
    }

    std::erase_if(context.allies, [](const UniquePtr<Ally>& ally) { return ally->isDead() || ally->isExpired(); });
}

void GameState::Playing::react(const RenderEvent&, GameFSM::EventControl& control)
{
    StateHelper::renderGameScene(control.context());
}