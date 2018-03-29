#pragma once
#include "NetworkCommandBase.h"
#include "Mixin_Base.h"
#include "EntityIDType.h"
#include "Helper_VariableSizeSerialization.h"
#include <vector>

namespace GEM::GameSim
{
	class UpdateSystemCommand : public NetworkCommand
	{
	public:
		ENTITY_ID_TYPE m_entityID;
		std::vector<MIXIN_ID_TYPE> m_mixins;
		std::vector<std::pair<MIXIN_ID_TYPE, std::string>> m_perMixinUpdates;

	private:

	public:

		UpdateSystemCommand() : m_entityID(0), NetworkCommand(0) {}
		/**!
		Packs up an entity update.
		Currently same idea is represented defferently in an engine and there is a Appearing/Regular distinction.
		In this context, resulting structure would be a Regular update
		*/
		UpdateSystemCommand(ENTITY_ID_TYPE entityID, std::vector<std::pair<MIXIN_ID_TYPE, std::string>> perMixinUpdates)
			: m_entityID(entityID), m_perMixinUpdates(perMixinUpdates), NetworkCommand(0) {}

		/**!
		Packs up an entity update.
		Currently same idea is represented defferently in an engine and there is a Appearing/Regular distinction.
		In this context, resulting structure would be an Appearing update
		*/
		UpdateSystemCommand(ENTITY_ID_TYPE entityID, std::vector<MIXIN_ID_TYPE> mixins, std::vector<std::pair<MIXIN_ID_TYPE, std::string>> perMixinUpdates)
			: m_entityID(entityID), m_mixins(mixins), m_perMixinUpdates(perMixinUpdates), NetworkCommand(0) {}


		template<class Archive>
		void save(Archive & archive) const
		{
			//Size of mixin-related vecors is 8-bit. So maximum ammount is 255
			assert(m_mixins.size() < 256);
			assert(m_perMixinUpdates.size() < 256);

			Helper::SaveVector<uint8_t>(archive, m_mixins);
			Helper::SaveVector<uint8_t>(archive, m_perMixinUpdates);
			archive(m_entityID);
		}

		template<class Archive>
		void load(Archive & archive)
		{
			Helper::LoadVector<uint8_t>(archive, m_mixins);
			Helper::LoadVector<uint8_t>(archive, m_perMixinUpdates);
			archive(m_entityID);
		}
		
	};
}