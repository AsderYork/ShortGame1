#pragma once
#include "NetworkCommandBase.h"
#include <memory>

namespace GEM::GameSim
{
	/**!
	Sometimes you need to send one command to a bunch of players. But command histories
	own the commands in a way, that cannot be shared.
	This interface contains a method, to make copies of a command in a uniform way
	*/
	class CopyableNetworkCommand : public NetworkCommand
	{
	public:

		CopyableNetworkCommand(uint8_t header) : NetworkCommand(header) {}
		/**!
		Creates a copy of a command and stores it in a unique_ptr
		*/
		virtual std::unique_ptr<NetworkCommand> copy() = 0;
	};
}