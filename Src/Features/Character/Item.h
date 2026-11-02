#pragma once

#include<variant>
#include <raylib.h>
#include "./Weapon/Gun/GunAttachment.h"
#include "./Weapon/Sword/SwordAttachment.h"

struct Attachment {
	std::variant<GunAttachment, SwordAttachment> attachment;
};

struct Item {
	Vector2 position;
	Attachment attachment;
};