#pragma once

#include "../GameContext.h"

class StateHelper {
public:
    static constexpr int CoinPerKill = 5;

    static void drawAmmo(GameContext& context);
    static void renderGameScene(GameContext& context);
    static void drawHud(GameContext& context);
    static void keepOutOfObstacles(const Map& map, Entity& entity, const Vector2& before);
    static UniquePtr<Ally> makeTrooperAlly(const Vector2& position);
	static UniquePtr<Ally> makeShadowClone(const Vector2& position, float lifetime);
	static int permanentAllies(const std::vector<UniquePtr<Ally>>& allies);
};