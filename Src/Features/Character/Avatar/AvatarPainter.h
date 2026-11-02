#pragma once

#include "Engine/Graphics.h"
#include "AvatarLook.h"
#include "AvatarPose.h"

class AvatarPainter {
public:
	static void drawBody(Engine::Graphics::Renderer& renderer, const AvatarPose& pose, const AvatarLook& look);
	static void drawUpper(Engine::Graphics::Renderer& renderer, const AvatarPose& pose, const AvatarLook& look);
};