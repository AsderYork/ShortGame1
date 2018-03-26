#pragma once
#include "Mixin_Base.h"


#include "NetworkCommandBase.h"
#include "NetworkCommandsProcessor.h"

#include "GS_EntityController.h"
#include "Helper_VariableSizeSerialization.h"

#include "ServerCommandDispatcher.h"
#include <cstdint>
#include <vector>
#include <array>
#include <cereal/access.hpp>
#include <memory>

#include <algorithm>
#include <boost\circular_buffer.hpp>

namespace GEM::GameSim
{
	/**!
	Ok. So it all comes down to events. It's clear, that we need some general way to communicate between
	client and server. Right now we have some parody of this. It's Updates system. But it works only for exchanging
	full states of entities, and have no mechanisms to perform rollbacks if needed. And they ARE needed.
	We allready had some parody on that in Events system, which could, actually, be used to perform what is needed
	but, it's just not working right now. So the tasl is to analize current system and what it's doing, analize
	flaws in design of a new system, analize what is needed from this system from a landscape system and then
	come up with a new design.

	First is waht is required.
	Right now "Updates" system just grab state from entities by special methods and transfer them to a server.
	Server checks the validity of those states and tries, end, if it's needed, updates current state of an entity.
	It also tries to predict that new state, but it handled in entities themself.

	What we need is a simple mechanism, that would allow to transfer events from one client to server and vice versa
	with some predictability mechanism to illiminate delay.

	And here it is. EventBase class. A main interface of all events. It should work for updates.
	And it should work terrain. But "Should" doesn't mean "will". And yeah, it's only "syncing" events.
	Some events in a game, probably, should not be sync'd. Like sprite creation or something. Could they use
	same event base, but without network? Well, they can't. And probably shouldn't. Becouse this EventBase
	is all about synchronization. So be it "SynchroEventBase" then. And every event in there will be used only
	with network.

	Mixins are actually comfortable with the way things are right now. They have a simple interface to
	send of their entire state, so that it can be rebuild on the other end. They use allmost same mechanism
	to send Updates. Question is: do we want to interfere with that system, or let it be?
	Well, analysis showed, that we can't let it be, if we want to swap everything for SynchroEvents.
	In that case we need to decouple state gathering for entity creation and for an update.

	And here we're are. At the square one. If events incapsulates action, it should have access
	to other systems. But it can't! Just becouse it's different systems. But what if for every event
	class there would be some EventProcessor, which have all the neccessary context to evaluate an action?
	In that case, after reciving an action, we should dispatch it to a processor.
	*/
	


	/**!
	Test classes
	*/

	class NetworkCommand_EntityUpdate : public NetworkCommand
	{
	protected:
		ENTITY_ID_TYPE m_entityID;
		std::vector<MIXIN_ID_TYPE> m_mixins;
		std::vector<std::pair<MIXIN_ID_TYPE, std::string>> m_perMixinUpdates;

	private:

		//!Default constructor just for Cereal
		NetworkCommand_EntityUpdate() : m_entityID(0), NetworkCommand(0) {}
		friend class cereal::access;
	public:

		/**!
		Packs up an entity update.
		Currently same idea is represented defferently in an engine and there is a Appearing/Regular distinction.
		In this context, resulting structure would be a Regular update
		*/
		NetworkCommand_EntityUpdate(ENTITY_ID_TYPE entityID, std::vector<std::pair<MIXIN_ID_TYPE, std::string>> perMixinUpdates)
			: m_entityID(entityID), m_perMixinUpdates(perMixinUpdates), NetworkCommand(0) {}

		/**!
		Packs up an entity update.
		Currently same idea is represented defferently in an engine and there is a Appearing/Regular distinction.
		In this context, resulting structure would be an Appearing update
		*/
		NetworkCommand_EntityUpdate(ENTITY_ID_TYPE entityID, std::vector<MIXIN_ID_TYPE> mixins, std::vector<std::pair<MIXIN_ID_TYPE, std::string>> perMixinUpdates)
			: m_entityID(entityID), m_mixins(mixins), m_perMixinUpdates(perMixinUpdates), NetworkCommand(0) {}


		template<class Archive>
		void save(Archive & archive) const
		{
			//Size of mixin-related vecors is 8-bit. So maximum ammount is 255
			assert(m_mixins.size() < 256);
			assert(m_perMixinUpdates.size() < 256);

			Helper::SaveVector<uint8_t>(archive, m_mixins);
			Helper::SaveVector<uint8_t>(archive, m_perMixinUpdates);
			ar(m_entityID);
		}

		template<class Archive>
		void load(Archive & archive)
		{
			Helper::LoadVector<uint8_t>(archive, m_mixins);
			Helper::LoadVector<uint8_t>(archive, m_perMixinUpdates);
			ar(m_entityID);
		}

		inline ENTITY_ID_TYPE getID() const { return m_entityID; }
		inline const std::vector<MIXIN_ID_TYPE>& getMixins() const { return m_mixins; }
		inline const std::vector<std::pair<MIXIN_ID_TYPE, std::string>>& getMixinUpdates() const { return m_perMixinUpdates; }

	};

	class EntityUpdatesNetworkProcessor
	{

	};

}