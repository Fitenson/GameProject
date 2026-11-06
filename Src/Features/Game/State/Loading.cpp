#include "Loading.h"
#include "../../Character/Avatar/AvatarFactory.h"
#include "../../Character/Weapon/WeaponFactory.h"

void GameState::Loading::update(GameFSM::FullControl& control)
{
    GameContext& context = control.context();

    context.dialog.reset();

    // The stage number picks the seed, so the same stage always gives the same map.
    context.world = makeUnique<World>(StageConfig::forStage(context.selectedStage));
    const Vector2 spawn = context.world->getMap().getSpawn();

    Character character;
    if (context.keepCharacter && context.player) {
        character = std::move(context.player->getCharacter());   // keeps upgrades and attachments
        character.heal(character.getMaxHp());
    }
    else {
        character = Character("User001", 1, AvatarFactory::createAvatar(context.selectedAvatar));

		WeaponType weaponType;

        switch (context.selectedAvatar) {
            case AvatarVariant::Assasin: weaponType = WeaponType::Sword; break;
            case AvatarVariant::Soldier: weaponType = WeaponType::Gun; break;
            case AvatarVariant::Tanker:  weaponType = WeaponType::HeavyGun; break;
            default: weaponType = WeaponType::Gun; break;
        }

        character.equip(WeaponFactory::create(weaponType));

        const float shieldRatio = character.getAvatar().getShieldRatio();
        if (shieldRatio > 0.0f) {
            character.equip(makeUnique<Shield>(character.getMaxHp() * shieldRatio));
        }
    }
    context.keepCharacter = false;

    context.player = makeUnique<Player>(std::move(character), spawn);
    context.world->setFocus(spawn);

    // Every stage counts its kills from zero. Scrap carries over.
    context.lastKills = 0;

    std::erase_if(context.allies, [](const UniquePtr<Ally>& ally) { return ally->isTemporary(); });

    // Allies from the previous stage start next to the new spawn point.
    float offset = 0.0f;
    for (UniquePtr<Ally>& ally : context.allies) {
        offset += 24.0f;
        ally->setPosition(Vector2{ spawn.x - offset, spawn.y + 10.0f });
        ally->setLeader(spawn);
    }

    std::vector<Target> noTargets;
    context.world->update(0.0f, context.player->getHitPosition(), noTargets);

    control.changeTo<GameState::Playing>();
}