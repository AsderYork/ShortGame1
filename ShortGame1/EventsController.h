#pragma once
#include <GameEvent_Base.h>
#include <EventsFactory.h>
#include <GS_EntityController.h>
#include <ClientCommandDispatcher.h>

#include <map>

namespace GEM
{/**!
 \brief Controls GameEvents
 */
	class GameEventsController
	{
	private:
		struct EventHolder {
			std::unique_ptr<GameSim::GameEvent> m_event;
			GameSim::NetworkCommandIDType m_networkid;
		};

		std::list<EventHolder> m_eventsList;

		GameSim::ClientCommandDispatcher * const m_commandDispatcher;

		GameSim::EntityController * const m_entityController;

		const GameSim::GameTime* const m_gameTime;

		GameEventsSystem_ClientProcessor* const m_eventsClientProcessor;

	public:
		
		inline GameEventsController(GameSim::EntityController* entityController, 
			const GameSim::GameTime* GameTimePtr,
			GameSim::ClientCommandDispatcher* commandDispatcher,
			GameEventsSystem_ClientProcessor* eventClientProcessor) :

			m_entityController(entityController),
			m_gameTime(GameTimePtr),
			m_commandDispatcher(commandDispatcher),
			m_eventsClientProcessor(eventClientProcessor)
		{
			GameSim::EventsFactory::RegisterAllEventTypes();
		}

		//Provide a way to check the state of an added Entity.
		struct EventTicket
		{
		private:
			GameSim::GameEvent * m_event;

			EventTicket(GameSim::GameEvent* gameevent) : m_event(gameevent) {}
			friend class GameEventsController;
		public:
			EventTicket(const EventTicket&) = delete;
			inline EventTicket(EventTicket&& other) {
				m_event = std::move(other.m_event);
			}
			~EventTicket()
			{
				if (m_event != nullptr) { m_event->m_refCounter--; }
			}

			inline GameSim::GameEvent::EventState getState() { return m_event->m_state; }

			/**
			Request to reject event execution.
			*/
			void StopEvent() {
				if (m_event->m_state == GameSim::GameEvent::EventState::Started)
				{
					m_event->m_state = GameSim::GameEvent::EventState::Rejected;
				}
				else if (m_event->m_state != GameSim::GameEvent::EventState::Completed || m_event->m_state != GameSim::GameEvent::EventState::Rejected) {
					m_event->m_state = GameSim::GameEvent::EventState::Stopping;
				}
			}
		};

		/**
		\brief Adds new event.
		*/
		EventTicket&& AddEvent(GameSim::EventIDType EventID, GameSim::ENTITY_ID_TYPE authorId);


		void ProcessEvents();


	};




}