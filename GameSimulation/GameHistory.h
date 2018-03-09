#pragma once
#include "EventBase.h"

#include <cstdint>
#include <vector>
#include <memory>
#include <array>
#include <list>
#include <sstream>
#include <limits>
#include <functional>

#include <cereal\cereal.hpp>
#include <cereal\archives\binary.hpp>
#include <cereal\types\vector.hpp>
#include <cereal\types\string.hpp>

namespace GEM::GameSim
{
	using SYNCING_EVENT_ID = uint32_t;

	struct OutOfSync_Packet
	{
		std::vector<SYNCING_EVENT_ID> DesyncronizedEvents;

		inline void Add(OutOfSync_Packet& packet)
		{ DesyncronizedEvents.insert(DesyncronizedEvents.end(), packet.DesyncronizedEvents.begin(), packet.DesyncronizedEvents.end());}

		template<class Archive>
		void serialize(Archive & archive)
		{
			archive(DesyncronizedEvents);
		}

	};

	struct InSync_Packet
	{
		SYNCING_EVENT_ID LastSynced = 0;

		inline void Add(InSync_Packet& packet)
		{
			if (packet.LastSynced > LastSynced) { LastSynced = packet.LastSynced; }
		}


		template<class Archive>
		void serialize(Archive & archive)
		{
			archive(LastSynced);
		}

	};

	struct SyncingUpdate_Packet;


	class GameHistoryController
	{
	public:
		/**!
		Events may be not a good thing for this system. They more of a one-time thing.
		Instead we allready have Update fcility in Mixins. So we can just pack this state-change
		as Update packet. That way we allready will have a way to generate them. But we still would have to add
		ValidateUpdate(UpdatePacket) method in mixins. Ok. But this info comes packed.
		*/
		struct StateChange
		{
			std::stringstream data;
			StateChange() = default;
			StateChange(const StateChange& other)
			{
				data.str(other.data.str());
			}

			template<class Archive>
			void save(Archive & archive) const
			{
				archive(data.str());
			}

			template<class Archive>
			void load(Archive & archive)
			{
				std::string instr;
				archive(instr);
				data.str(instr);
			}


		};
		struct EventData
		{
			std::unique_ptr<StateChange> Event;
			SYNCING_EVENT_ID UniuqeEventID;
			bool AssumedToBeInSync = true;
		};

	private:
		//Events in this array are stored circularly
		std::vector<EventData> m_updates;
		std::size_t m_nextPos = 0;//Points to the last added event
		std::size_t m_historyEnd;

		//Determines if the history of events where changed and events are actually should be re-applied
		bool m_doNeedReaplying = false;

		/*! Counts all events in accending order. IMPORTANT! The average amount of events should be counted for more precise choose of type size
		Starts with 1, becouse 0 is reserved for no events.
		*/
		SYNCING_EVENT_ID m_lastID = 1;

		/**!
		Holds the last ID, that were recived as synced. Used deal with problem, where server resends ID after we're allready
		removed event.
		*/
		SYNCING_EVENT_ID m_lastSyncedID = 0;

		std::size_t getShiftedPos(std::size_t curr, int shift) const
		{
			return ((curr + m_updates.size()) + (shift % (int)m_updates.size())) % m_updates.size();
		}


		void AddNewUpdateWithID(std::unique_ptr<StateChange>&& Update, SYNCING_EVENT_ID EventID)
		{
			if (m_updates[m_nextPos].Event != nullptr)
			{
				throw std::exception("Circular Storage Overflow");
			}
			m_updates[m_nextPos].Event = std::move(Update);
			m_updates[m_nextPos].UniuqeEventID = EventID;
			m_updates[m_nextPos].AssumedToBeInSync = true;


			m_nextPos = (m_nextPos + 1) % m_updates.size();
		}
	public:

		GameHistoryController(std::size_t StorageSize = 400)
		{
			m_updates.resize(StorageSize);
			m_historyEnd = getShiftedPos(0, -1);
		}

		/**!
		Adds new Update in a history. Returns this exact update in a form, ready to be send out
		*/
		SyncingUpdate_Packet AddNewUpdate(const StateChange Update);

		/**!
		Recives array of OutOfSync events and checks if they where predicted or not.
		If they where predicted, then we're just ignoring it. If it was not predicted, then we mark entire history for
		re-applying.

		Vector with OOS id's MUST be sorted from the oldest events to the newest! And cannot be empty!
		*/
		void ProcessOutOfSync(const OutOfSync_Packet& ids)
		{
			if (ids.DesyncronizedEvents.size() == 0) { return; }

			//Why one? Becouse we're starting from the Next position, and so the position before it is first one with actual event
			int TmpShift = 1;
			for (auto OOSIter = ids.DesyncronizedEvents.rbegin(); OOSIter != ids.DesyncronizedEvents.rend(); OOSIter++)
			{
				auto ShiftedPos = getShiftedPos(m_nextPos, -TmpShift);
				while (ShiftedPos != m_historyEnd)
				{
					if (m_updates[ShiftedPos].AssumedToBeInSync)
					{
						m_doNeedReaplying = true;
						return;
					}

				}
				if (TmpShift == m_historyEnd)
				{
					//If we're here, then we have checked every event in history and still havn't found and event, with listed ID
					throw std::exception("Listed in OOS event is not present in history!");
				}
			}
		}

		/**!
		Removes confirmed events from the history
		*/
		void ProcessInSync(const InSync_Packet& LastSynced)
		{
			if (LastSynced.LastSynced == 0) { return; }
			if(m_lastSyncedID == LastSynced.LastSynced) { return; }

			bool StartRemovingEvents = false;

			int i = 1;
			std::size_t NewHistoryEnd;

			for (auto shift = getShiftedPos(m_nextPos, -i); shift != m_historyEnd; shift = getShiftedPos(m_nextPos, -(++i)))
			{
				if (StartRemovingEvents)
				{
					m_updates[shift].Event.reset();
				}
				else
				{
					if (m_updates[shift].UniuqeEventID == LastSynced.LastSynced)
					{
						m_updates[shift].Event.reset();
						m_updates[shift].UniuqeEventID = 0;
						NewHistoryEnd = shift;//This is gonna be new end of the history!
						StartRemovingEvents = true;
					}
				}
			}

			if (!StartRemovingEvents)
			{
				throw std::exception("InSync event, provided to history is not present!");
			}
			m_lastSyncedID = LastSynced.LastSynced;
			m_historyEnd = NewHistoryEnd;

		}

		/**!
		Re-applies the events, that are assumed to happen.
		\param[in] ApplyMethod A method, that will be applied to every event, that are assumed to happen
		This method recives A pointer to an event, that should be applied and ID of that event. This method
		also must return true, if event was applied successfully and false, if event wasn't applied.

		\note events, for which ApplyMethod returned false are assumed to be potentially-OutOfSync.
		*/
		void ReApplyUpdates(std::function<bool(StateChange*)> ApplyMethod)
		{
			if (!m_doNeedReaplying) { return; }
			int Shift = 0;
			std::size_t Pos = getShiftedPos(m_historyEnd, Shift);
			for (; Pos != m_nextPos; getShiftedPos(m_historyEnd, ++Shift))
			{
				//History have changed, so we obviously can't really trust our previous sugestions, about what may or may not happen,
				//So re-apply all the events
				m_updates[Pos].AssumedToBeInSync = ApplyMethod(m_updates[Pos].Event.get());
			}
			m_doNeedReaplying = false;
		}

		/**!
		Return true if history is considered accurate, false history is inaccurate applying of a last known confirmed
		state is needed.

		\note GameHistory require caller to apply this correct state if needed, before using ReApplyEvents!
		*/
		bool IsHistoryAccurate() const { return !m_doNeedReaplying; }

	};

	struct SyncingUpdate_Packet
	{
		GameHistoryController::StateChange UpdateData;
		SYNCING_EVENT_ID UniuqeEventID;

		SyncingUpdate_Packet(const GameHistoryController::StateChange& Data, const SYNCING_EVENT_ID ID) :
			UpdateData(Data), UniuqeEventID(ID)
		{}
		SyncingUpdate_Packet() = default;

		template<class Archive>
		void serialize(Archive & archive)
		{
			archive(UniuqeEventID, UpdateData);
		}
	};


}