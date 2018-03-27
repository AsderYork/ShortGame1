#pragma once
#include <cstdint>

namespace GEM::GameSim
{
	using NetworkCommandIDType = uint64_t;

	class NetworkCommand
	{
	public:
		//Determines processor that have sent/should process this message
		uint8_t m_header;
		NetworkCommandIDType m_uniqueID;

		NetworkCommand(uint8_t header) : m_header(header) {}
		NetworkCommand() : m_header(0) {}
	};
}