#include "stdafx.h"
#include "GS_Client.h"

#include "SharedDataPackets.h"
#include "UpdateSystem_Command.h"

#include <cereal\archives\binary.hpp>
#include <cereal\types\vector.hpp>
#include <cereal\types\utility.hpp>
#include <cereal\types\string.hpp>
#include <cereal\types\chrono.hpp>

namespace GEM
{
	void GS_Client::SimulationStarted()
	{
		m_updatesProcessor.AddControlledEntity(m_playerCharacterID);
		m_chunkDispatcher.getController().createNewLoader([&]() {return dynamic_cast<GameSim::Mixin_Movable*>(m_entities.GetEntity(m_playerCharacterID)->GetMixinByID(GameSim::Mixin_Movable::MixinID))->getPos(); });
	}

	bool GS_Client::Tick(float Delta, cereal::BinaryInputArchive& archive, std::stringstream& OutputStream, bool ArchiveIsEmpty)
	{
		
		if (!ArchiveIsEmpty)
		{
			std::vector<GameSim::ServerCommandPack> ServerPacks;
			try
			{
				while (true)
				{
					GameSim::ServerCommandPack NewPack;
					NewPack.SerializeOut(archive, m_dispatcher.getProcessorsTable());
					ServerPacks.emplace_back(std::move(NewPack));
				}
			}
			catch (...)
			{
			}


			if (!m_timeIsSet)//Check if it's the first update of session
			{//It is
				//m_simulationTime = ServerPacks.back().time;
				m_timeIsSet = true;
			}
			else
			{
				/*if (GameTimeToSeconds(m_gs.getGameTime() - Updates.back().second) > 1.0f)
				{//Client can't keep up with the server updates!
					//return 0;
				}*/
			}


			m_dispatcher.ProcessCommands(std::move(ServerPacks));
		}


		//If player have not yet been set, that means that simulation is not initialized enough for all that history
		//manipulation stuff. So we'll just perform one tick of simulation and skip everything else
		if (m_entities.GetEntitiesCount() == 0) { return GameSimulation::Tick(Delta); }


		m_chunkDispatcher.Process(&m_dispatcher);


		//Perform basic simulation tick
		auto Retval = GameSimulation::Tick(Delta);
	
		m_updatesProcessor.GatherStatesOfControlledEntities(&m_dispatcher);
		{
			cereal::BinaryOutputArchive OutputAr(OutputStream);
			m_dispatcher.GatherResults(getGameTime()).SerealizeIn(OutputAr, m_dispatcher.getProcessorsTable());
		}
		

		return Retval;
	}

}