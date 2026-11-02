#pragma once

#include <array>
#include <string>
#include <vector>

#include "Engine/Utils.h"
#include "Engine/DataStructure.h"
#include "../Weapon.h"
#include "./Attachment/GunPart.h"
#include "./Attachment/Magazine.h"
#include "../AmmoInfo.h"
#include "Bullet.h"
#include "GunStats.h"
#include "GunAttachment.h"

class Gun : public Weapon
{
public:
    typedef Iterator<
        Engine::DataStructure::ListNode<
        GunPart*,
        Engine::DataStructure::ListType::Singly
        >
    > ListIterator;

    Gun();   // the standard, modular gun
    Gun(const std::string& name, float cooldown, float damageMultiplier, bool modular);
    ~Gun();

    Gun(const Gun&) = delete;
    Gun& operator=(const Gun&) = delete;

    // Attach a part at the end of the chain.
    // The magazine should be the first part.
    void addPart(GunPart* part);

    AmmoInfo getAmmoInfo() const override;

    ListIterator begin() const;
    ListIterator end() const;

    bool isModular() const;
    bool hasAttachment(GunAttachment attachment) const;
    int getAttachmentCost(GunAttachment attachment) const;
    bool setAttachment(GunAttachment attachment, bool value);

    static bool isMuzzleDevice(GunAttachment attachment);

    HandPose getHandPose(const Vector2& origin, const Vector2& direction) const override;

    void renderHeld(
        Engine::Graphics::Renderer& renderer,
        const Vector2& playerPosition,
        const Vector2& direction
    ) const override;

    void renderEffects(
        Engine::Graphics::Renderer& renderer
    ) const override;

protected:
    void onAttack(const AttackContext& context) override;
    void onUpdate(float deltaTime, const AttackContext& context) override;

private:
    static constexpr float HolderGap = 8.0f;
    static constexpr float BaseReloadTime = 1.6f;

    Engine::DataStructure::List<
        GunPart*,
        Engine::DataStructure::ListType::Singly
    > parts;

    Engine::DataStructure::Queue<Bullet*> reserve;
    GunStats stats;
    float tipOffset = HolderGap;

    std::vector<Bullet*> flying;

    float flashTimer = 0.0f;
    float recoil = 0.0f;

    bool reloading = false;
    float reloadTime = BaseReloadTime;
    float reloadLeft = 0.0f;

    bool modular = true;
    std::array<bool, AttachmentCount> installed{};

    Magazine* getMagazine() const;

    bool takeBullet(Bullet*& out, bool unlimited);
    void recycle(Bullet* bullet);

    void startReload();
    void finishReload();
    void rebuild();
    void assemble();
};