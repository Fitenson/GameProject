#pragma once

struct AvatarLook {
	Color outline{ 22, 28, 22, 255 };
	Color uniform{ 66, 91, 49, 255 };
	Color dark{ 42, 59, 35, 255 };
	Color armor{ 92, 83, 53, 255 };
	Color pouch{ 118, 103, 65, 255 };
	Color helmet{ 48, 73, 39, 255 };
	Color highlight{ 87, 111, 65, 255 };
	Color boots{ 30, 33, 29, 255 };
	Color glove{ 40, 44, 38, 255 };
	Color accent{ 92, 83, 53, 255 };

	float bulk = 1.0f;
	bool shoulderPads = false;
	bool backpack = true;
	bool scarf = false;
};