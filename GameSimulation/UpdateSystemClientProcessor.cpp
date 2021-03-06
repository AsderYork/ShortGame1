#include "UpdateSystemClientProcessor.h"
#include "ClientCommandDispatcher.h"
#include "Helper_VariableSizeSerialization.h"
#include <algorithm>
#include <cereal\archives\binary.hpp>
#include <cereal\types\string.hpp>
#include <cereal\types\utility.hpp>

namespace GEM::GameSim
{
	bool UpdateSystemClientProcessor::ApplyState(EntityBase* ent, std::vector<std::pair<MIXIN_ID_TYPE, std::string>> state)
	{		
		bool UpdateFullyAccepted = true;
		for (auto& mixin : state)
		{
			std::stringstream sstream(mixin.second);
			cereal::BinaryInputArchive ar(sstream);
			ent->GetMixinByID(mixin.first)->ApplyUpdate(ar);
		}
		return UpdateFullyAccepted;
	}

	void UpdateSystemClientProcessor::GatherStatesOfControlledEntities(ClientCommandDispatcher* dispatcher)
	{
		for (auto& ent : m_controlledEntities)
		{
			if (ent.ptr == nullptr) 
			{
				ent.ptr = m_gameSim->m_entities.GetEntity(ent.id).lock().get();
			}

			std::vector<std::pair<MIXIN_ID_TYPE, std::string>> UpdateForEntity;
			for (auto& mixin : ent.ptr->getAllMixins())
			{
				std::stringstream newupdate;
				if (mixin->NeedsUpdate())
				{
					{
						cereal::BinaryOutputArchive ar(newupdate);
						mixin->SendUpdate(ar, Mixin_base::UpdateReason::REGULAR);
						UpdateForEntity.emplace_back(mixin->getMixinID(), newupdate.str());
						newupdate.str(std::string());
					}
				}
			}

			//Do not send empty updates!
			if (UpdateForEntity.size() == 0) { continue; }

			dispatcher->InsertPerformedCommand(std::make_unique<UpdateSystemCommand>(ent.id, UpdateForEntity));
		}
	}

	void UpdateSystemClientProcessor::AddControlledEntity(ENTITY_ID_TYPE id)
	{
		auto it = std::upper_bound(m_controlledEntities.begin(), m_controlledEntities.end(), id);
		m_controlledEntities.emplace(it, id);
	}
	void UpdateSystemClientProcessor::RemoveControlledEntity(ENTITY_ID_TYPE id)
	{
		auto it = std::lower_bound(m_controlledEntities.begin(), m_controlledEntities.end(), id);
		if (it != m_controlledEntities.end()) { m_controlledEntities.erase(it); }
	}



	void UpdateSystemClientProcessor::ConfirmCommand(const NetworkCommand * Command)
	{
		auto CommandRecast = static_cast<const UpdateSystemCommand*>(Command);
		auto it = std::lower_bound(m_controlledEntities.begin(), m_controlledEntities.end(), CommandRecast->m_entityID);
		if (it != m_controlledEntities.end() && it->id == CommandRecast->m_entityID)
		{
			//It's a controlled entity, so just overwrite it's LastConfirmedUpdate
			it->lastConfirmedUpdate = CommandRecast->m_perMixinUpdates;
			if (!it->IsInitialized)
			{
				it->IsInitialized = true;
				it->IsConfirmedStateApplied = true;

				auto EntIt = m_gameSim->m_entities.GetEntity(CommandRecast->m_entityID).lock();
				ApplyState(EntIt.get(), it->lastConfirmedUpdate);

				bool UpdateFullyAccepted = true;

			}
		}
		else
		{
			//This is not controlled entity.
			//Which means that it's state was correctly applied by \c ApplyCommand.
		}


	}

	void UpdateSystemClientProcessor::RejectCommand(const NetworkCommand * Command)
	{
		auto CommandRecast = static_cast<const UpdateSystemCommand*>(Command);
		auto it = std::lower_bound(m_controlledEntities.begin(), m_controlledEntities.end(), CommandRecast->m_entityID);
		if (it == m_controlledEntities.end())
		{
			//A command was rejected, but entity which command was rejected is notcontrolled
			//Which means that it couldn't sent out anything, that could be rejected
			//There's defnetly a problem
			throw std::exception("Non-controlled entity recived a RejectCommand!");
		}
		else
		{
			//If controlled entity recived rejection, just apply last confirmed state to it
			//But rejections have tendency to be grouped. So to avoid re-applying same state multiple times
			//In a row, we first check, if it wasn't allready applied in a current pass
			if (!it->IsConfirmedStateApplied)
			{
				it->IsConfirmedStateApplied = true;

				auto EntIt = m_gameSim->m_entities.GetEntity(CommandRecast->m_entityID).lock();
				ApplyState(EntIt.get(), it->lastConfirmedUpdate);
			}
		}
	}


	void UpdateSystemClientProcessor::EndNetworkProcessing()
	{
		for (auto& ent : m_controlledEntities)
		{
			ent.IsConfirmedStateApplied = false;
		}
	}

	bool UpdateSystemClientProcessor::ApplyCommand(const NetworkCommand * Command, GameTime PacketTime)
	{
		auto CommandRecast = static_cast<const UpdateSystemCommand*>(Command);
		std::shared_ptr<EntityBase> EntIt;

		if (!CommandRecast->m_mixins.empty())
		{
			EntIt = m_gameSim->AddEntity(CommandRecast->m_entityID, CommandRecast->m_mixins).lock();
		}
		else
		{
			EntIt = m_gameSim->m_entities.GetEntity(CommandRecast->m_entityID).lock();
		}

		auto it = std::lower_bound(m_controlledEntities.begin(), m_controlledEntities.end(), CommandRecast->m_entityID);
		if (it != m_controlledEntities.end() && it->id == CommandRecast->m_entityID)
		{
			//This is a controlled entity. Which means that it doesn't have to apply Server's state immediately
			//Instead it's state will be applied only when server discards it's state
			//So just skip it and let Confirm and Reject do all the work
			//TODO UpdateSystem is really not a thing to translate player-made changes.
			for (auto& mixin : CommandRecast->m_perMixinUpdates)
			{
				if (mixin.first == Mixin_Movable::MixinID) { continue; }
				std::stringstream sstream(mixin.second);
				cereal::BinaryInputArchive ar(sstream);
				EntIt->GetMixinByID(mixin.first)->ApplyUpdate(ar);
			}
			return true;
		}

		return ApplyState(EntIt.get(), CommandRecast->m_perMixinUpdates);
	}
	uint8_t UpdateSystemClientProcessor::getIdOfProcessor() const
	{
		return uint8_t();
	}
}