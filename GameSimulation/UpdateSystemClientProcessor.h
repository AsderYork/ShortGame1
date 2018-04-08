#pragma once
#include "NetworkCommandsProcessor.h"
#include "GameSimulation.h"
#include "ClientCommandDispatcher.h"

namespace GEM::GameSim
{
	class ClientCommandDispatcher;

	class UpdateSystemClientProcessor : public NetworkExchangeProcessor
	{
	private:

		GameSimulation * m_gameSim;

		struct ControlledEntity
		{
			ENTITY_ID_TYPE id;
			EntityBase* ptr;
			bool IsConfirmedStateApplied;
			std::vector<std::pair<MIXIN_ID_TYPE, std::string>> lastConfirmedUpdate;
			

			bool operator<(const ControlledEntity& other) { return id < other.id; }
			bool operator<(const ENTITY_ID_TYPE& otherid) { return id < otherid; }
			ControlledEntity(ENTITY_ID_TYPE entid) : id(entid), IsConfirmedStateApplied(false), ptr(nullptr) {}

			friend bool operator<(const ENTITY_ID_TYPE& l, const ControlledEntity& r)
			{
				return l < r.id;
			}
		};

		
		//Entities that are being controlled by this player
		std::vector<ControlledEntity>	m_controlledEntities;

		bool ApplyState(EntityBase* ent, std::vector<std::pair<MIXIN_ID_TYPE, std::string>> state);
	public:

		UpdateSystemClientProcessor(GameSimulation* GameSim) : m_gameSim(GameSim) {};

		void GatherStatesOfControlledEntities(ClientCommandDispatcher* dispatcher);

		void AddControlledEntity(ENTITY_ID_TYPE id);
		void RemoveControlledEntity(ENTITY_ID_TYPE id);

		virtual void RollbackCommand(const NetworkCommand * Command) override;
		virtual bool ReapplyCommand(const NetworkCommand * Command) override;
		virtual void ConfirmCommand(const NetworkCommand * Command) override;
		virtual void RejectCommand(const NetworkCommand * Command) override;
		virtual void SerializeCommand(cereal::BinaryOutputArchive & ar, const NetworkCommand * Command) override;
		virtual std::unique_ptr<NetworkCommand> deserializeCommand(cereal::BinaryInputArchive & ar) override;
		virtual void EndNetworkProcessing() override;

		virtual bool ApplyCommand(const NetworkCommand * Command, GameTime PacketTime) override;
		virtual uint8_t getIdOfProcessor() const override;
	};
}