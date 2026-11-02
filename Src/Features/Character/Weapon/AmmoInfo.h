#pragma once

struct AmmoInfo {
	bool used = false;
	int loaded = 0;
	int capacity = 0;
	int standby = 0;
	bool reloading = false;
	float progress = 0.0f;
};