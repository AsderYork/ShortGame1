#pragma once
#include "CopyableNetworkCommand.h"
#include "GameTime.h"

namespace GEM::GameSim
{

	/**!
	These commands allows server to controll GameTime on on the clients.
	Only server can send this commands.
	*/
	class GameTimeSystemCommand : public CopyableNetworkCommand
	{
	public:
		GameTime NewTime;
		float NewMultiplayer;


		GameTimeSystemCommand(GameTime Time, float Multiplayer) : CopyableNetworkCommand(2), NewTime(Time), NewMultiplayer(Multiplayer){}
		GameTimeSystemCommand() : CopyableNetworkCommand(2), NewTime(0), NewMultiplayer(0) {}

		virtual std::unique_ptr<NetworkCommand> copy();

		template<class Archive>
		void save(Archive & archive) const
		{
			archive(NewTime);
			archive(NewMultiplayer);
		}

		template<class Archive>
		void load(Archive & archive)
		{
			archive(NewTime);
			archive(NewMultiplayer);
		}

		~GameTimeSystemCommand() {}

	};

}