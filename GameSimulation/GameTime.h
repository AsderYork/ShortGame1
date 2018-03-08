#pragma once
#include <cstdint>

namespace GEM::GameSim
{
	using GameTime = uint64_t;

	inline GameTime SecondsToGameTimeInterval(float t) { return static_cast<GameTime>(t * 100000); }
	inline float GameTimeToSeconds(GameTime t) { return static_cast<float>(t) / 100000.0f; }
}