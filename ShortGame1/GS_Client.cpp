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
	GS_Client::GS_Client() : m_timeIsSet(false),
		m_updatesProcessor(this),
		m_landPhys((&m_physics)) 
	{
		m_dispatcher.AddProcessor(&m_updatesProcessor);
		m_dispatcher.AddProcessor(&m_chunkDispatcher.getProcessor());

		m_chunkDispatcher.m_chunks.RegisterListener(&m_landPhys);
	}
	void GS_Client::SimulationStarted()
	{
		m_updatesProcessor.AddControlledEntity(m_playerCharacterID);
		m_chunkDispatcher.getController().createNewLoader([&]() {return dynamic_cast<GameSim::Mixin_Movable*>(m_entities.GetEntity(m_playerCharacterID)->GetMixinByID(GameSim::Mixin_Movable::MixinID))->getPos(); });
	}

	bool GS_Client::Tick(float Delta, std::stringstream& InputStream, std::stringstream& OutputStream)
	{
		if (InputStream.rdbuf()->in_avail() > 0)
		{
			cereal::BinaryInputArchive archive(InputStream);
			std::vector<GameSim::ServerCommandPack> ServerPacks;
			try
			{
				while (InputStream.rdbuf()->in_avail() > 0)
				{
					std::size_t PacketSize = 0;
					archive(PacketSize);
					if (InputStream.rdbuf()->in_avail() < PacketSize)
					{//If current packet is not completely recived
						//Unget PacketSize and stop reading
						for (int i = 0; i < sizeof(std::size_t); i++) { InputStream.unget(); }
						//This packet should be processed on the next tick
						//When the rest will be recived

						LOGCATEGORY("GS_Client/MainLoop").info("Uncomplited datapack recived! It calims that it's size is %s but we have only %i!", PacketSize, InputStream.rdbuf()->in_avail());
					}

					GameSim::ServerCommandPack NewPack;
					NewPack.SerializeOut(archive, m_dispatcher.getProcessorsTable());
					ServerPacks.emplace_back(std::move(NewPack));
				}
			}
			catch (cereal::Exception& e)
			{
				LOGCATEGORY("GS_Client/MainLoop").info("CerealException:%s", e.what());
				//If we're here, then archive just ended.
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

			static auto CurrTime = std::chrono::steady_clock::now();
			static int CoughtPackets = 0;
			CoughtPackets += ServerPacks.size();
			if (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - CurrTime).count() >= 1)
			{
				CurrTime = std::chrono::steady_clock::now();
				LOGCATEGORY("CoughtPackets").info("Cought %i packets in last second", CoughtPackets);
				CoughtPackets = 0;

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