#pragma once
#include <cstdint>

namespace GEM::GameSim
{
	using GameTime = int64_t;


	inline GameTime SecondsToGameTimeInterval(float t) { return static_cast<GameTime>(round(t * 100000)); }
	inline float GameTimeToSeconds(GameTime t) { return t / 100000.0f; }
}