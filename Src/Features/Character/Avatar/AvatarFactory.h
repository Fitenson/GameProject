#pragma once

#include "Engine/Core.h"
#include "Avatar.h"

class AvatarFactory {
public:
	static UniquePtr<Avatar> createAvatar(AvatarVariant variant);
};