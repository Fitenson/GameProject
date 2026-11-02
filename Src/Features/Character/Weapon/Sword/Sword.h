#pragma once

#include <array>
#include <string>
#include "Engine/Utils.h"
#include "Engine/DataStructure.h"
#include "../Weapon.h"
#include "./Attachment/SwordPart.h"
#include "SwordStats.h"
#include "SwordAttachment.h"

struct Hand {
	float side;                      // +1 = right hand, -1 = left hand
	float swingTime = 0.0f;
	float centerAngle = 0.0f;        // aim angle when this hand's swing started
	Vector2 origin{ 0.0f, 0.0f };    // hand position in the world
};

class Sword : public Weapon {
public:
	typedef Iterator<
		Engine::DataStructure::ListNode<
		SwordPart*,
		Engine::DataStructure::ListType::Singly
		>
	> ListIterator;

	Sword();
	~Sword();

	Sword(const Sword&) = delete;
	Sword& operator=(const Sword&) = delete;

	ListIterator begin() const;
	ListIterator end() const;

	void addPart(SwordPart* part);

	// Attachments
	bool hasAttachment(SwordAttachment attachment) const;
	int getAttachmentCost(SwordAttachment attachment) const;
	bool setAttachment(SwordAttachment attachment, bool value);

	HandPose getHandPose(const Vector2& playerPosition, const Vector2& direction) const override;
	void renderHeld(Engine::Graphics::Renderer& renderer, const Vector2& playerPosition, const Vector2& direction) const override;
	void renderEffects(Engine::Graphics::Renderer& renderer) const override;

protected:
	void onAttack(const AttackContext& context) override;
	void onUpdate(float deltaTime, const AttackContext& context) override;

private:
	static constexpr float HolderGap = 8.0f;
	static constexpr float HandOffset = 7.0f;
	static constexpr float RestSpread = 0.45f;
	static constexpr float GripDistance = HolderGap + 9.0f;

	Engine::DataStructure::List<SwordPart*, Engine::DataStructure::ListType::Singly> parts;
	SwordStats stats;
	float tipOffset = HolderGap;

	std::array<Hand, 2> hands{ Hand{ -1.0f }, Hand{ 1.0f } };
	size_t nextHand = 1;

	std::array<bool, SwordAttachmentCount> installed{};   // indexed by SwordAttachment

	void rebuild();
	void assemble();
	static Vector2 playerHand(const Vector2& playerPosition, const Vector2& direction, float side);
	float swingAngle(const Hand& hand) const;
	float handAngle(const Hand& hand, const Vector2& direction) const;
};