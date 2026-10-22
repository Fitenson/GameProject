#include "Random.h"

SeededRandom::SeededRandom(uint64_t seed) : state(seed) {}

uint32_t SeededRandom::next() {
	state += 0x9E3779B97F4A7C15ull;
	uint64_t z = state;
	z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ull;
	z = (z ^ (z >> 27)) * 0x94D049BB133111EBull;
	return static_cast<uint32_t>((z ^ (z >> 31)) >> 32);
}

float SeededRandom::unit() { return static_cast<float>(next() >> 8) / 16777216.0f; }   // [0, 1)
float SeededRandom::real(float low, float high) { return low + (high - low) * unit(); }
int SeededRandom::integer(int low, int high) {                                          // inclusive
	return low + static_cast<int>(next() % static_cast<uint32_t>(high - low + 1));
}
bool SeededRandom::chance(float probability) { return unit() < probability; }