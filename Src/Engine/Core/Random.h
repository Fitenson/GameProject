#pragma once

#include <cstdint>

class SeededRandom {
public:
	SeededRandom(uint64_t seed);

	uint32_t next();
	float unit();
	float real(float low, float high);
	int integer(int low, int high);
	bool chance(float probability);

private:
	uint64_t state;
};