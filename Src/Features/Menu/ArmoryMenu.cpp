
#include <algorithm>
#include <string>
#include <utility>

#include "ArmoryMenu.h"
#include "../Character/Weapon/Gun/Gun.h"
#include "../Character/Weapon/Sword/Sword.h"

namespace {
    constexpr Color Background{ 18, 34, 38, 255 };
    constexpr Color PanelColor{ 28, 52, 58, 255 };
    constexpr Color Accent{ 20, 160, 133, 255 };
    constexpr Color Gold{ 245, 200, 60, 255 };
    constexpr Color Muted{ 170, 190, 190, 255 };

    constexpr int CostDamage = 50;
    constexpr int CostArmor = 40;
    constexpr int CostHeal = 30;

    constexpr float ArmorCap = 75.0f;
    constexpr float ArmorIncrease = 5.0f;
    constexpr float DamageIncrease = 3.0f;

    const GunAttachment Attachments[] = {
        GunAttachment::Scope,
        GunAttachment::Compensator,
        GunAttachment::Suppressor
    };

    std::string describe(GunAttachment attachment)
    {
        switch (attachment) {
        case GunAttachment::Scope:
            return "+250 bullet range";

        case GunAttachment::Compensator:
            return "halves the recoil";

        case GunAttachment::Suppressor:
            return "no muzzle flash, bullets 5% slower";
        }

        return "";
    }

    bool isMuzzleDevice(GunAttachment attachment)
    {
        return attachment != GunAttachment::Scope;
    }
}

// ---------- Constructor ----------

ArmoryMenu::ArmoryMenu(
    Character& character,
    std::vector<UniquePtr<Ally>>& allies,
    int& coins,
    std::array<size_t, AttachmentCount>& stock,
    std::string backLabel)
    : player(character),
    allies(allies),
    coin(coins),
    stock(stock),
    backLabel(std::move(backLabel))
{
}

// ---------- Helpers ----------

void ArmoryMenu::addText(
    const std::string& value,
    Vector2 position,
    int size,
    Color color)
{
    texts.push_back(Text{ value, position, size, color });
}

void ArmoryMenu::addControl(
    Rectangle bounds,
    const std::string& label,
    bool enabled,
    std::function<void()> action,
    bool highlighted)
{
    controls.push_back(
        Control{ bounds, label, enabled, highlighted, std::move(action) }
    );
}

bool ArmoryMenu::spend(int cost)
{
    if (cost < 0 || coin < cost) {
        notify("Not enough coins (need " + std::to_string(cost) + ")");
        return false;
    }

    coin -= cost;
    return true;
}

void ArmoryMenu::notify(const std::string& text)
{
    message = text;
    messageTimer = 2.5f;
}

// ---------- Layout ----------

void ArmoryMenu::build()
{
    controls.clear();
    texts.clear();
    panels.clear();

    const float w = static_cast<float>(GetScreenWidth());
    const float h = static_cast<float>(GetScreenHeight());

    addText("ARMORY", Vector2{ 40.0f, 20.0f }, 44, WHITE);

    addText(
        "Scrap: " + std::to_string(coin),
        Vector2{ w - 240.0f, 30.0f },
        30,
        Gold
    );

    addControl(
        Rectangle{ 40.0f, h - 76.0f, 220.0f, 50.0f },
        backLabel,
        true,
        [this] { closeRequested = true; }
    );

    addControl(
        Rectangle{ 40.0f, 90.0f, 170.0f, 42.0f },
        "WEAPONS",
        true,
        [this] { tab = Tab::Weapons; },
        tab == Tab::Weapons
    );

    addControl(
        Rectangle{ 224.0f, 90.0f, 170.0f, 42.0f },
        "ALLIES",
        true,
        [this] { tab = Tab::Allies; },
        tab == Tab::Allies
    );

    if (tab == Tab::Weapons) {
        buildWeapons(w, h);
    }
    else {
        buildAllies(w, h);
    }
}

// ---------- Weapons ----------

void ArmoryMenu::buildWeapons(float w, float h)
{
    const size_t count = player.getWeaponCount();

    if (count == 0) {
        addText(
            "No weapons equipped.",
            Vector2{ 40.0f, 170.0f },
            24,
            Muted
        );
        return;
    }

    selectedWeapon = std::min(selectedWeapon, count - 1);

    // Weapon selection list.
    for (size_t i = 0; i < count; ++i) {
        Weapon* weapon = player.getWeapon(i);

        if (weapon == nullptr) {
            continue;
        }

        addControl(
            Rectangle{
                40.0f,
                160.0f + static_cast<float>(i) * 58.0f,
                290.0f,
                50.0f
            },
            weapon->getName() + "   Lv " +
            std::to_string(weapon->getLevel()),
            true,
            [this, i] { selectedWeapon = i; },
            i == selectedWeapon
        );
    }

    Weapon* weapon = player.getWeapon(selectedWeapon);

    if (weapon == nullptr) {
        addText(
            "Invalid weapon selection.",
            Vector2{ 360.0f, 170.0f },
            24,
            Muted
        );
        return;
    }

    const float x = 360.0f;
    const float y = 160.0f;

    panels.push_back(
        Rectangle{ x, y, w - x - 40.0f, h - y - 100.0f }
    );

    addText(
        weapon->getName(),
        Vector2{ x + 24.0f, y + 16.0f },
        34,
        WHITE
    );

    addText(
        "Level " + std::to_string(weapon->getLevel()) +
        " / " + std::to_string(Weapon::MaxLevel),
        Vector2{ x + 24.0f, y + 58.0f },
        22,
        Muted
    );

    addText(
        "Damage bonus: +" +
        std::to_string(20 * (weapon->getLevel() - 1)) + "%",
        Vector2{ x + 24.0f, y + 88.0f },
        22,
        Muted
    );

    // Weapon level upgrade.
    if (!weapon->isLevelMax()) {
        const int cost = weapon->getUpgradeCost();

        addControl(
            Rectangle{ x + 24.0f, y + 124.0f, 300.0f, 46.0f },
            "UPGRADE   (" + std::to_string(cost) + ")",
            coin >= cost,
            [this, weapon] {
                // Recheck the current state when clicked.
                if (weapon->isLevelMax()) {
                    notify("Weapon is already at maximum level");
                    return;
                }

                const int currentCost = weapon->getUpgradeCost();

                if (!spend(currentCost)) {
                    return;
                }

                weapon->setLevel(weapon->getLevel() + 1);

                notify(
                    weapon->getName() + " upgraded to level " +
                    std::to_string(weapon->getLevel())
                );
            }
        );
    }
    else {
        addControl(
            Rectangle{ x + 24.0f, y + 124.0f, 300.0f, 46.0f },
            "MAX LEVEL",
            false,
            [] {}
        );
    }

    const Rectangle button0{
        x + 420.0f,
        0.0f,
        240.0f,
        44.0f
    };

    // ---------- Sword attachments ----------

    if (Sword* sword = dynamic_cast<Sword*>(weapon)) {
        addText(
            "ATTACHMENTS",
            Vector2{ x + 24.0f, y + 196.0f },
            24,
            Accent
        );

        float rowY = y + 236.0f;

        for (size_t i = 0; i < SwordAttachmentCount; ++i) {
            const SwordAttachment attachment =
                static_cast<SwordAttachment>(i);

            const bool installed = sword->hasAttachment(attachment);
            const int cost = sword->getAttachmentCost(attachment);

            addText(
                attachmentName(attachment),
                Vector2{ x + 24.0f, rowY + 2.0f },
                24,
                installed ? Gold : WHITE
            );

            addText(
                attachmentDescription(attachment),
                Vector2{ x + 24.0f, rowY + 28.0f },
                16,
                Muted
            );

            Rectangle button = button0;
            button.y = rowY;

            if (installed) {
                addControl(
                    button,
                    "REMOVE  (+" + std::to_string(cost) + ")",
                    true,
                    [this, sword, attachment, cost] {
                        if (!sword->hasAttachment(attachment)) {
                            notify("Attachment is not installed");
                            return;
                        }

                        if (!sword->setAttachment(attachment, false)) {
                            notify("Could not remove attachment");
                            return;
                        }

                        coin += cost;

                        notify(
                            attachmentName(attachment) + " removed"
                        );
                    }
                );
            }
            else {
                addControl(
                    button,
                    "INSTALL  (" + std::to_string(cost) + ")",
                    coin >= cost,
                    [this, sword, attachment] {
                        if (sword->hasAttachment(attachment)) {
                            notify("Attachment is already installed");
                            return;
                        }

                        const int currentCost =
                            sword->getAttachmentCost(attachment);

                        if (!spend(currentCost)) {
                            return;
                        }

                        if (!sword->setAttachment(attachment, true)) {
                            coin += currentCost;
                            notify("Installation failed; scrap refunded");
                            return;
                        }

                        notify(
                            attachmentName(attachment) + " installed"
                        );
                    }
                );
            }

            rowY += 62.0f;
        }

        return;
    }

    // ---------- Gun attachments ----------

    Gun* gun = dynamic_cast<Gun*>(weapon);

    if (gun == nullptr || !gun->isModular()) {
        addText(
            "This weapon has no free attachment slots.",
            Vector2{ x + 24.0f, y + 200.0f },
            20,
            Muted
        );
        return;
    }

    int stored = 0;

    for (size_t quantity : stock) {
        stored += static_cast<int>(quantity);
    }

    addText(
        "ATTACHMENTS",
        Vector2{ x + 24.0f, y + 196.0f },
        24,
        Accent
    );

    addText(
        "In storage: " + std::to_string(stored),
        Vector2{ x + 220.0f, y + 200.0f },
        18,
        stored > 0 ? Gold : Muted
    );

    float rowY = y + 236.0f;

    for (GunAttachment attachment : Attachments) {
        const bool installed = gun->hasAttachment(attachment);
        const int cost = gun->getAttachmentCost(attachment);
        const size_t stockIndex = indexOf(attachment);
        const size_t owned = stock[stockIndex];

        const bool otherMuzzle =
            isMuzzleDevice(attachment) &&
            !installed &&
            (
                gun->hasAttachment(GunAttachment::Compensator) ||
                gun->hasAttachment(GunAttachment::Suppressor)
                );

        addText(
            attachmentName(attachment),
            Vector2{ x + 24.0f, rowY + 2.0f },
            24,
            installed ? Gold : WHITE
        );

        addText(
            describe(attachment) +
            (owned > 0
                ? "   (owned x" + std::to_string(owned) + ")"
                : ""),
            Vector2{ x + 24.0f, rowY + 28.0f },
            16,
            owned > 0 ? Gold : Muted
        );

        Rectangle button = button0;
        button.y = rowY;

        if (installed) {
            addControl(
                button,
                "REMOVE",
                true,
                [this, gun, attachment, stockIndex] {
                    if (!gun->hasAttachment(attachment)) {
                        notify("Attachment is not installed");
                        return;
                    }

                    if (!gun->setAttachment(attachment, false)) {
                        notify("Could not remove attachment");
                        return;
                    }

                    ++stock[stockIndex];

                    notify(
                        attachmentName(attachment) +
                        " moved to storage"
                    );
                }
            );
        }
        else if (otherMuzzle) {
            addControl(
                button,
                "MUZZLE SLOT TAKEN",
                false,
                [] {}
            );
        }
        else {
            const std::string label = owned > 0
                ? "INSTALL  (owned)"
                : "BUY & INSTALL  (" + std::to_string(cost) + ")";

            addControl(
                button,
                label,
                owned > 0 || coin >= cost,
                [this, gun, attachment, cost, stockIndex] {
                    if (gun->hasAttachment(attachment)) {
                        notify("Attachment is already installed");
                        return;
                    }

                    if (
                        isMuzzleDevice(attachment) &&
                        (
                            gun->hasAttachment(GunAttachment::Compensator) ||
                            gun->hasAttachment(GunAttachment::Suppressor)
                            )
                        ) {
                        notify("Muzzle slot is already occupied");
                        return;
                    }

                    const bool useStock = stock[stockIndex] > 0;

                    if (!useStock && !spend(cost)) {
                        return;
                    }

                    if (!gun->setAttachment(attachment, true)) {
                        if (!useStock) {
                            coin += cost;
                        }

                        notify("Installation failed; purchase refunded");
                        return;
                    }

                    if (useStock) {
                        --stock[stockIndex];
                    }

                    notify(attachmentName(attachment) + " installed");
                }
            );
        }

        rowY += 62.0f;
    }

    addText(
        "Changing parts reloads the gun.",
        Vector2{ x + 24.0f, rowY + 4.0f },
        16,
        Muted
    );
}

// ---------- Allies ----------

void ArmoryMenu::buildAllies(float w, float h)
{
    if (allies.empty()) {
        addText(
            "You have no allies yet.",
            Vector2{ 40.0f, 170.0f },
            26,
            WHITE
        );

        addText(
            "Open the cage on the road to recruit one.",
            Vector2{ 40.0f, 206.0f },
            20,
            Muted
        );

        return;
    }

    for (size_t i = 0; i < allies.size(); ++i) {
        Ally* ally = allies[i].get();

        if (ally == nullptr) {
            continue;
        }

        Character& character = ally->getCharacter();

        const float y = 150.0f + static_cast<float>(i) * 150.0f;
        const float x = 40.0f;

        panels.push_back(
            Rectangle{ x, y, w - 80.0f, 136.0f }
        );

        addText(
            character.getName(),
            Vector2{ x + 20.0f, y + 12.0f },
            30,
            WHITE
        );

        addText(
            "HP " +
            std::to_string(static_cast<int>(character.getHp())) +
            "/" +
            std::to_string(static_cast<int>(character.getMaxHp())) +
            "    Damage " +
            std::to_string(static_cast<int>(character.getDamage())) +
            "    Armor " +
            std::to_string(static_cast<int>(character.getArmor())),
            Vector2{ x + 20.0f, y + 50.0f },
            20,
            Muted
        );

        Weapon* weapon = character.getWeaponCount() > 0
            ? character.getWeapon(0)
            : nullptr;

        if (weapon != nullptr) {
            addText(
                weapon->getName() + "  Lv " +
                std::to_string(weapon->getLevel()),
                Vector2{ x + 20.0f, y + 78.0f },
                20,
                Gold
            );
        }

        const float bx = x + 360.0f;

        // DAMAGE upgrade.
        addControl(
            Rectangle{ bx, y + 14.0f, 230.0f, 44.0f },
            "DAMAGE +3  (" + std::to_string(CostDamage) + ")",
            coin >= CostDamage,
            [this, ally] {
                Character& target = ally->getCharacter();

                if (!spend(CostDamage)) {
                    return;
                }

                target.getDamage() += DamageIncrease;

                notify("Ally damage increased");
            }
        );

        // ARMOR upgrade.
        addControl(
            Rectangle{ bx + 244.0f, y + 14.0f, 230.0f, 44.0f },
            "ARMOR +5  (" + std::to_string(CostArmor) + ")",
            coin >= CostArmor &&
            character.getArmor() < ArmorCap,
            [this, ally] {
                Character& target = ally->getCharacter();

                if (target.getArmor() >= ArmorCap) {
                    notify("Ally armor is already at the cap");
                    return;
                }

                if (!spend(CostArmor)) {
                    return;
                }

                const float increase = std::min(
                    ArmorIncrease,
                    ArmorCap - target.getArmor()
                );

                target.getArmor() += increase;

                notify("Ally armor increased");
            }
        );

        // HEAL upgrade.
        addControl(
            Rectangle{ bx, y + 74.0f, 230.0f, 44.0f },
            "HEAL  (" + std::to_string(CostHeal) + ")",
            coin >= CostHeal &&
            character.getHp() < character.getMaxHp(),
            [this, ally] {
                Character& target = ally->getCharacter();

                if (target.getHp() >= target.getMaxHp()) {
                    notify("Ally already has full HP");
                    return;
                }

                if (!spend(CostHeal)) {
                    return;
                }

                target.heal(target.getMaxHp());

                notify("Ally fully healed");
            }
        );

        // Ally weapon upgrade.
        if (weapon != nullptr && !weapon->isLevelMax()) {
            const int cost = weapon->getUpgradeCost();

            addControl(
                Rectangle{ bx + 244.0f, y + 74.0f, 230.0f, 44.0f },
                "WEAPON UP  (" + std::to_string(cost) + ")",
                coin >= cost,
                [this, weapon] {
                    if (weapon->isLevelMax()) {
                        notify("Weapon is already at maximum level");
                        return;
                    }

                    const int currentCost = weapon->getUpgradeCost();

                    if (!spend(currentCost)) {
                        return;
                    }

                    weapon->setLevel(weapon->getLevel() + 1);

                    notify(
                        weapon->getName() + " upgraded to level " +
                        std::to_string(weapon->getLevel())
                    );
                }
            );
        }
        else {
            addControl(
                Rectangle{ bx + 244.0f, y + 74.0f, 230.0f, 44.0f },
                "WEAPON MAX",
                false,
                [] {}
            );
        }
    }
}

// ---------- Screen ----------

void ArmoryMenu::update(
    float deltaTime,
    NavigationStack& navigation)
{
    messageTimer = std::max(0.0f, messageTimer - deltaTime);

    // Build once before processing input.
    build();

    hovered = -1;

    const Vector2 mouse = GetMousePosition();

    for (size_t i = 0; i < controls.size(); ++i) {
        if (!CheckCollisionPointRec(mouse, controls[i].bounds)) {
            continue;
        }

        hovered = static_cast<int>(i);

        if (
            controls[i].enabled &&
            IsMouseButtonPressed(MOUSE_BUTTON_LEFT)
            ) {
            // Copy the callback because it may change the menu state.
            const std::function<void()> action = controls[i].action;

            if (action) {
                action();
            }

            // Do not process multiple controls for one click.
            break;
        }
    }

    if (closeRequested || IsKeyPressed(KEY_ESCAPE)) {
        closeRequested = false;
        navigation.pop();
    }
}

void ArmoryMenu::render(Engine::Graphics::Renderer& renderer)
{
    // Do not rebuild controls here; update() already did that.
    renderer.clear(Background);

    for (const Rectangle& panel : panels) {
        renderer.drawRectangle(panel, PanelColor);
        renderer.drawRectangleOutline(panel, 2.0f, Accent);
    }

    for (const Text& text : texts) {
        renderer.drawText(
            text.value,
            text.position,
            text.size,
            text.color
        );
    }

    for (size_t i = 0; i < controls.size(); ++i) {
        const Control& control = controls[i];
        const bool hover = static_cast<int>(i) == hovered;

        const Color fill = control.enabled
            ? (
                hover
                ? Color{ 225, 250, 244, 255 }
                : WHITE
                )
            : Color{ 110, 125, 128, 255 };

        renderer.drawRectangle(control.bounds, fill);

        if (control.highlighted) {
            renderer.drawRectangleOutline(
                control.bounds,
                4.0f,
                Gold
            );
        }

        const int size = 20;
        const int width = renderer.measureText(control.label, size);

        renderer.drawText(
            control.label,
            Vector2{
                control.bounds.x +
                    (control.bounds.width - static_cast<float>(width)) * 0.5f,
                control.bounds.y +
                    (control.bounds.height - static_cast<float>(size)) * 0.5f
            },
            size,
            control.enabled
            ? Accent
            : Color{ 70, 80, 82, 255 }
        );
    }

    if (messageTimer > 0.0f) {
        const unsigned char alpha = static_cast<unsigned char>(
            255.0f * std::min(1.0f, messageTimer)
            );

        const int width = renderer.measureText(message, 24);

        renderer.drawText(
            message,
            Vector2{
                (static_cast<float>(GetScreenWidth()) -
                    static_cast<float>(width)) * 0.5f,
                static_cast<float>(GetScreenHeight()) - 60.0f
            },
            24,
            Color{ Gold.r, Gold.g, Gold.b, alpha }
        );
    }
}
