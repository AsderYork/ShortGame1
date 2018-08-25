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
		m_gameTimeProcessor(this),
		m_landPhys((&m_physics)) 
	{
		m_dispatcher.AddProcessor(&m_updatesProcessor); 
		m_dispatcher.AddProcessor(&m_chunkDispatcher.getProcessor());
		m_dispatcher.AddProcessor(&m_gameTimeProcessor);


		m_chunkDispatcher.m_chunks.RegisterListener(&m_landPhys);
	}
	void GS_Client::SimulationStarted()
	{
		m_updatesProcessor.AddControlledEntity(m_playerCharacterID);
		m_chunkDispatcher.getController().createNewLoader([&]() {return dynamic_cast<GameSim::Mixin_Movable*>(m_entities.GetEntity(m_playerCharacterID).lock()->GetMixinByID(GameSim::Mixin_Movable::MixinID))->getPos(); });
	}

	bool GS_Client::Tick(float Delta, GEM::NetworkConnection* connection)
	{
		if (connection->ReciveData().size() > 0)
		{
			
			std::vector<GameSim::ServerCommandPack> ServerPacks;
			try
			{
				while (connection->ReciveData().size() > 0)
				{
					auto ReciveStream = connection->getReciveStream();
					cereal::BinaryInputArchive archive(ReciveStream);

					static std::size_t PacketSize = 0;

					if (PacketSize == 0)
					{
						archive(PacketSize);
					}
					
					if (connection->ReciveData().size() < PacketSize + sizeof(std::size_t))
					{//If current packet is not completely recived
						//This packet should be processed on the next tick
						//When the rest will be recived
						LOGCATEGORY("GS_Client/MainLoop").info("Uncomplited datapack recived! It calims that it's size is %i but we have only %i!", PacketSize, connection->ReciveData().size() - 8);
						break;
					}

					GameSim::ServerCommandPack NewPack;
					NewPack.SerializeOut(archive, m_dispatcher.getProcessorsTable());

					ServerPacks.emplace_back(std::move(NewPack));

					PacketSize = 0;
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
		//Otherwise player is set

		if (!m_stateInitialized)
		{//If this is the first time when the player is set, then it's a FirstTick Event!
			for (auto& callback : m_firstTickEventCallbacks) { if (!callback()) { return false; } }
			m_stateInitialized = true;
			m_firstTickEventCallbacks.clear();
		}


		m_chunkDispatcher.Process(&m_dispatcher);


		//Perform basic simulation tick
		auto Retval = GameSimulation::Tick(Delta);
	
		m_updatesProcessor.GatherStatesOfControlledEntities(&m_dispatcher);
		{
			std::stringstream stream;
			cereal::BinaryOutputArchive OutputAr(stream);
			m_dispatcher.GatherResults(getGameTime()).SerealizeIn(OutputAr, m_dispatcher.getProcessorsTable());
			connection->SendData() += stream.str();
		}
		

		return Retval;
	}

	void GS_Client::SimulationShutdown()
	{
		m_chunkDispatcher.Clear();
	}

}