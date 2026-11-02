#include <algorithm>
#include <utility>
#include "Gun.h"
#include "./Attachment/Receiver.h"
#include "./Attachment/Scope.h"
#include "./Attachment/Compensator.h"
#include "./Attachment/Suppressor.h"
#include "./Attachment/Barrel.h"

namespace
{
    Bullet* makeBullet()
    {
        return new Bullet(
            Vector2{ 0.0f, 0.0f },
            Vector2{ 1.0f, 0.0f },
            0.0f,
            0.0f,
            0.0f
        );
    }
}

Gun::Gun()
    : Weapon("Gun", 0.15f, 1.0f, true), modular(true)
{
    assemble();
}

// Subclasses such as HeavyGun pass modular = false and build their own parts.
Gun::Gun(
    const std::string& name,
    float cooldown,
    float damageMultiplier,
    bool modular
)
    : Weapon(name, cooldown, damageMultiplier, true), modular(modular)
{
}

Gun::~Gun()
{
    parts.clear();

    for (Bullet* bullet : flying) {
        delete bullet;
    }
}

bool Gun::isMuzzleDevice(GunAttachment attachment) {
    return attachment == GunAttachment::Compensator || attachment == GunAttachment::Suppressor;
}

// Physical order: magazine, receiver, scope (on top), barrel, muzzle device (at the tip).
void Gun::assemble()
{
    parts.clear();

    addPart(new Magazine());
    addPart(new Receiver());
    if (hasAttachment(GunAttachment::Scope))       addPart(new Scope());
    addPart(new Barrel());
    if (hasAttachment(GunAttachment::Compensator)) addPart(new Compensator());
    if (hasAttachment(GunAttachment::Suppressor))  addPart(new Suppressor());
}

bool Gun::isModular() const { return modular; }

bool Gun::hasAttachment(GunAttachment attachment) const
{
    return installed[indexOf(attachment)];
}

int Gun::getAttachmentCost(GunAttachment attachment) const
{
    switch (attachment) {
    case GunAttachment::Scope:       return 40;
    case GunAttachment::Compensator: return 60;
    case GunAttachment::Suppressor:  return 80;
    default:                         return 0;
    }
}

bool Gun::setAttachment(GunAttachment attachment, bool value)
{
    if (!modular) return false;

    // Only one muzzle device fits at the tip.
    if (value && isMuzzleDevice(attachment)) {
        for (GunAttachment other : { GunAttachment::Compensator, GunAttachment::Suppressor }) {
            if (other != attachment && hasAttachment(other)) return false;
        }
    }

    if (hasAttachment(attachment) == value) return true;

    installed[indexOf(attachment)] = value;
    assemble();
    return true;
}

Gun::ListIterator Gun::begin() const
{
    return parts.getIterator();
}

Gun::ListIterator Gun::end() const
{
    return parts.getIterator().end();
}

Magazine* Gun::getMagazine() const
{
    ListIterator iterator = begin();

    if (iterator == end()) {
        return nullptr;
    }

    // getCurrent() returns a ListNode pointer.
    // getData() returns the GunPart* stored inside the node.
    return dynamic_cast<Magazine*>(iterator.getCurrent()->getValue());
}

void Gun::addPart(GunPart* part)
{
    if (part == nullptr) {
        return;
    }

    if (!parts.pushBack(part)) {
        return;
    }

    rebuild();
}

void Gun::rebuild()
{
    stats = GunStats{};
    tipOffset = HolderGap;

    Gun::ListIterator iterator = begin();

    while (iterator != end()) {
        GunPart* part = iterator.getCurrent()->getValue();

        if (part != nullptr) {
            part->modify(stats);
            tipOffset += part->length();
        }

        ++iterator;
    }

    reloading = false;
    reloadLeft = 0.0f;
    reloadTime = BaseReloadTime / std::max(0.1f, stats.reloadSpeed);

    reserve.clear();

    if (Magazine* magazine = getMagazine()) {
        reserve.setMax(magazine->getCapacity());

        while (!reserve.isFull()) {
            reserve.enqueue(makeBullet());
        }
    }
    else {
        reserve.setMax(0);
    }
}

bool Gun::takeBullet(Bullet*& out, bool unlimited)
{
    Magazine* magazine = getMagazine();

    if (magazine == nullptr || unlimited) {
        if (unlimited) {
            reloading = false;  // Overdrive cancels reload animation
        }
        out = makeBullet();
        return true;
    }

    if (reloading) {
        return false;
    }

    out = magazine->fire();

    if (out == nullptr) {
        startReload();
        return false;
    }

    if (magazine->isEmpty()) {
        startReload();
    }

    return true;
}

void Gun::recycle(Bullet* bullet)
{
    if (bullet == nullptr) {
        return;
    }

    if (reserve.enqueue(bullet)) {
        return;
    }

    delete bullet;
}

void Gun::startReload()
{
    reloading = true;
    reloadLeft = reloadTime;
}

void Gun::finishReload()
{
    if (Magazine* magazine = getMagazine()) {
        magazine->swapQueue(reserve);
        magazine->refill();
    }

    reloading = false;
    reloadLeft = 0.0f;
}

void Gun::onAttack(const AttackContext& context)
{
    Bullet* bullet = nullptr;

    if (!takeBullet(bullet, context.unlimitedAmmo)) {
        return;
    }

    flashTimer = 0.05f;
    recoil = stats.recoil;

    const Vector2 tip{
        context.playerPosition.x + context.direction.x * tipOffset,
        context.playerPosition.y + context.direction.y * tipOffset
    };

    bullet->fire(
        tip,
        context.direction,
        stats.bulletSpeed,
        damageOf(context),
        stats.bulletRange
    );

    flying.push_back(bullet);
}

void Gun::onUpdate(
    float deltaTime,
    const AttackContext& context
)
{
    if (Magazine* magazine = getMagazine()) {
        if (reloading) {
            reloadLeft -= deltaTime;

            if (reloadLeft <= 0.0f) {
                finishReload();
            }
        }
        else if (context.reloadRequested && !magazine->isFull() && !context.unlimitedAmmo) {
            startReload();
        }
    }

    flashTimer = std::max(0.0f, flashTimer - deltaTime);
    recoil = std::max(0.0f, recoil - 80.0f * deltaTime);

    for (Bullet* bullet : flying) {
        bullet->update(deltaTime);

        if (!bullet->isAlive()) {
            continue;
        }

        const Vector2 position = bullet->getPosition();

        for (Enemy* target : context.targets) {
            if (target->isDead()) {
                continue;
            }

            const Vector2 targetPosition = target->getHitPosition();
            const float reach = target->getHitRadius() + Bullet::radius;

            const float dx = targetPosition.x - position.x;
            const float dy = targetPosition.y - position.y;

            if (dx * dx + dy * dy <= reach * reach) {
                target->takeDamage(bullet->getDamage());
                bullet->kill();
                break;
            }
        }
    }

    std::erase_if(flying, [this](Bullet* bullet) {
        if (bullet->isAlive()) {
            return false;
        }

        recycle(bullet);
        return true;
        });
}

AmmoInfo Gun::getAmmoInfo() const
{
    const Magazine* magazine = getMagazine();

    if (magazine == nullptr) {
        return AmmoInfo{};
    }

    const float progress = reloading
        ? std::clamp(1.0f - reloadLeft / reloadTime, 0.0f, 1.0f)
        : 0.0f;

    return AmmoInfo{
        true,
        magazine->getLoaded(),
        magazine->getCapacity(),
        reserve.size(),
        reloading,
        progress
    };
}

HandPose Gun::getHandPose(const Vector2& origin, const Vector2& direction) const
{
    const GunMount mount{ origin, direction, HolderGap, recoil };   // pointAt() subtracts the recoil

    HandPose pose;
    pose.hasRight = pose.hasLeft = true;
    pose.right = mount.pointAt(HolderGap + 5.0f, 1.0f);    // trigger hand
    pose.left = mount.pointAt(HolderGap + 12.0f, -1.0f);   // support hand

    if (reloading) {
        // The support hand swings to the magazine and back over the reload time.
        const float t = std::clamp(1.0f - reloadLeft / reloadTime, 0.0f, 1.0f);
        const float reach = std::sin(t * 3.14159265f);
        const Vector2 magazine = mount.pointAt(HolderGap + 3.0f, -8.0f);
        pose.left.x += (magazine.x - pose.left.x) * reach;
        pose.left.y += (magazine.y - pose.left.y) * reach;
    }
    return pose;
}

void Gun::renderHeld(
    Engine::Graphics::Renderer& renderer,
    const Vector2& playerPosition,
    const Vector2& direction
) const
{
    float offset = HolderGap;

    Gun::ListIterator iterator = begin();

    while (iterator != end()) {
        GunPart* part = iterator.getCurrent()->getValue();

        if (part != nullptr) {
            part->render(
                renderer,
                GunMount{ playerPosition, direction, offset, recoil }
            );

            offset += part->length();
        }

        ++iterator;
    }

    if (flashTimer > 0.0f && stats.flashScale > 0.0f) {
        const GunMount tipMount{
            playerPosition,
            direction,
            tipOffset,
            recoil
        };

        const Vector2 flash = tipMount.pointAt(tipOffset + 4.0f);

        renderer.drawCircle(
            flash,
            9.0f * stats.flashScale,
            Color{ 255, 170, 40, 255 }
        );

        renderer.drawCircle(
            flash,
            5.0f * stats.flashScale,
            Color{ 255, 245, 180, 255 }
        );
    }

    if (reloading) {
        const float progress = std::clamp(
            1.0f - reloadLeft / reloadTime,
            0.0f,
            1.0f
        );

        const Rectangle back{
            playerPosition.x - 14.0f,
            playerPosition.y + 20.0f,
            28.0f,
            4.0f
        };

        renderer.drawRectangle(back, Color{ 20, 20, 20, 200 });

        renderer.drawRectangle(
            Rectangle{
                back.x,
                back.y,
                back.width * progress,
                back.height
            },
            Color{ 245, 200, 60, 255 }
        );

        renderer.drawRectangleOutline(back, 1.0f, BLACK);
    }
}

void Gun::renderEffects(Engine::Graphics::Renderer& renderer) const
{
    for (const Bullet* bullet : flying) {
        bullet->render(renderer);
    }
}