#pragma once
#include "EventBase.h"

#include <cstdint>
#include <vector>
#include <memory>
#include <array>
#include <list>

namespace GEM::GameSim
{
	using SYNCING_EVENT_ID = uint32_t;

	struct OutOfSync_Packet
	{
		std::vector<SYNCING_EVENT_ID> DesyncronizedEvents;
	};




	class SimulationCicrularStorage
	{
	public:
		struct EventData
		{
			std::unique_ptr<EventBase> Event;
			SYNCING_EVENT_ID UniuqeEventID;
			bool AssumedToBeInSync = true;
		};
	private:
		//Events in this array are stored circularly
		std::vector<EventData> m_events;
		std::size_t m_nextPos = 0;//Points to the last added event
		std::size_t m_historyEnd;

		//Determines if the history of events where changed and events are actually should be re-applied
		bool m_doNeedReaplying = false;

		std::size_t getShiftedPos(std::size_t curr, int shift) const
		{
			return ((curr + m_events.size()) + (shift % m_events.size())) % m_events.size();
		}
	public:
		
		SimulationCicrularStorage(std::size_t StorageSize=100)
		{
			m_events.resize(StorageSize);
			m_historyEnd = getShiftedPos(0, -1);
		}

		void AddNewEvent(std::unique_ptr<EventBase>&& Event, SYNCING_EVENT_ID EventID)
		{
			if (m_events[m_nextPos].Event != nullptr)
			{
				throw std::exception("Circular Storage Overflow");
			}
			m_events[m_nextPos].Event = std::move(Event);
			m_events[m_nextPos].UniuqeEventID = EventID;
			m_events[m_nextPos].AssumedToBeInSync = true;


			m_nextPos = (m_nextPos + 1) % m_events.size();
		}

		/**!
		Recives array of OutOfSync events and checks if they where predicted or not.
		If they where predicted, then we're just ignoring it. If it was not predicted, then we mark entire history for
		re-applying.

		Vector with OOS id's MUST be sorted from the oldest events to the newest! And cannot be empty!
		*/
		void ProcessOutOfSync(const std::vector<SYNCING_EVENT_ID>& ids)
		{
			if (ids.size() == 0) { throw std::exception("OOS-vector can't be empty!"); }

			//Why one? Becouse we're starting from the Next position, and so the position before it is first one with actual event
			int TmpShift = 1;
			for (auto OOSIter = ids.rbegin(); OOSIter != ids.rend(); OOSIter++)
			{
				auto ShiftedPos = getShiftedPos(m_nextPos, -TmpShift);
				while (ShiftedPos != m_historyEnd)
				{
					if (m_events[ShiftedPos].AssumedToBeInSync)
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
		void ProcessInSync(SYNCING_EVENT_ID LastSynced)
		{
			bool StartRemovingEvents = false;
			for (int i = 1; i < m_events.size(); i++)
			{
				if (StartRemovingEvents)
				{
					m_events[getShiftedPos(m_nextPos, -i)].Event.reset();
				}
				else
				{
					if (m_events[getShiftedPos(m_nextPos, -i)].UniuqeEventID == LastSynced)
					{
						m_events[getShiftedPos(m_nextPos, -i)].Event.reset();
						m_historyEnd = getShiftedPos(m_nextPos, -i);//This is gonna be new end of the history!
						StartRemovingEvents = true;
					}
				}
			}

			if(!StartRemovingEvents)
			{
				throw std::exception("InSync event, provided to history is not present!");
			}
		}

		/**!
		Re-applies the events, that are assumed to happen.
		\param[in] ApplyMethod A method, that will be applied to every event, that are assumed to happen
		This method recives A pointer to an event, that should be applied and ID of that event. This method
		also must return true, if event was applied successfully and false, if event wasn't applied.

		\note events, for which ApplyMethod returned false are assumed to be potentially-OutOfSync.
		*/
		void ReApplyEvents(bool(ApplyMethod)(EventBase*, SYNCING_EVENT_ID))
		{
			if (!m_doNeedReaplying) { return; }
			for (int Shift = 0, Pos = getShiftedPos(m_historyEnd, Shift); Pos != m_nextPos; getShiftedPos(m_historyEnd, ++Shift))
			{
				//History have changed, so we obviously can't really trust our previous sugestions, about what may or may not happen,
				//So re-apply all the events
				m_events[Pos].AssumedToBeInSync = ApplyMethod(m_events[Pos].Event.get(), m_events[Pos].UniuqeEventID);
			}
			m_doNeedReaplying = false;
		}



	};



}