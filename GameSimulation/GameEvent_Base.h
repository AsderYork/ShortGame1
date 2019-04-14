#pragma once
#include "GameTime.h"
#include "GameObject.h"
#include "GS_EntityController.h"

#include <limits>
#include <assert.h>
#include <optional>
#include <Hasher.h>
#undef max


#include <list>

namespace GEM
{
	class GameEventsController;//Controller might be from the client
	class GameEventsSystem_ClientProcessor;
}

namespace GEM::GameSim
{
	using EventIDType = int;
	using ObjectIDType = int;

	class EventsFactory;

	/**
	/brief Encapsulates in-game interractions.
	/warn Every event must include GameEventIDHolder as it's base CRTP;	
	*/
	class GameEvent
	{
	public:

		enum EventState : uint8_t
		{
			Started,//Event was just created.
			Sended,//Event was sended to the server.
			AwaitingServer,//If this is a long event, this state will be set, while client waits for server second confirmtion.
			Stopping,//Event was asked to stop, but the server still doesn't know about it and might do it anyway.
			Rejected,//Server rejected this event ither by a request from a client or by any other reason.
			Completed//Event was complited as intended

		};


	private:
		/**!
		\brief Provides safe controlled access to a ref counter;
		*/
		struct Counter {
		private:
			int8_t counter = 1;
		public:
			inline decltype(counter) operator++(int)
			{
				assert(counter != std::numeric_limits<decltype(counter)>::max());//Too much listeners on one event!
				return counter++;
			}

			inline decltype(counter) operator--(int)
			{
				assert(counter > 0);//Can't be less then zero after decrement!
				return counter--;
			}

			inline operator decltype(counter)() const{ return counter; }
		};

		Counter m_refCounter;//Counts refrences. An event can be destroyed ONLY if nothing is refrencing it.

		ObjectIDType m_AuthorID;//ID of an object, that created that event;
		GameTime m_CreationTime;//A time at which this event was created;

		//True if event can require multiple frames for comlition, false if it's only require one frame
		bool m_isEventLong = false;

		inline void Initialize(ObjectIDType AuthorID, GameTime currTime) {
			m_AuthorID = AuthorID;
			m_CreationTime = currTime;
		}

		EventState m_state = EventState::Started;

		friend class GameEventsController;
		friend class GameEventsSystem_ClientProcessor;
		friend class EventsFactory;
	public:

		virtual EventIDType id() const = 0;
		inline bool isEventLong() { return m_isEventLong; }


		inline auto getAuthorID() const { return m_AuthorID; }
		inline auto getCreationType() const { return m_CreationTime; }

		inline EventState getState() const { return m_state; }
		
		/**
		Stores all the things, that might be needed on Validity check, applying or rollback of event
		*/
		struct Dependecies
		{
			EntityBase* thisObject = nullptr;//A pointer to author object, if there is one.
			EntityController* entityController;
			//LandscapeInterface;
			inline Dependecies(EntityBase* _thisObject, EntityController* _entityController) :
				thisObject(_thisObject),
				entityController(_entityController)
			{}

		};
		/**
		\brief Checks if event is withtin game rules.
		\param [in] Deps - const pointers to all the systems, that can be utilized to evaluate event validity.
		\returns True of event is valid, false otherwise. Returns nothing if answer is not ready yet.
		*/
		virtual std::optional<bool> CheckValidity(const Dependecies& Deps) = 0;

		/**
		\brief Applies an event
		\param[in] Deps Outside dependecies
		*/
		virtual void ApplyEvent(Dependecies& Deps) = 0;

		/**!
		\brief Reverts the changes done by this event.
		If event is applied before it was confirmed, it might be requred to revert this event or
		at least return the status quo to all affected systems.
		*/
		virtual void RollbackEvent(Dependecies& Deps) = 0;

		virtual ~GameEvent() {}

	};

	/**
	Provides Additional methods, that might be required for the object.
	*/
	template<typename base>
	struct StaticGameEvent : public GameEvent {
		//! Returns ID of this Event, derived from it's name hashed.
		inline static EventIDType staticid() { return Helper::Hasher<EventIDType>::Hash(__FUNCTION__, "GameEvent"); }
		inline EventIDType id() const { return staticid(); }
	};

	/**
	\page Eventsandstates States of events
	Things are acting on each other. Players cutting trees, wolves biting players and so on. So there must be a way for a wolf to
	somehow tell the player, that it is bitten in a way, so that the server could check, if wolf really did bite the player as it claims to.
	For all of this there is events! For example, when a wolf bites the player, it posts a special events, where it describes, which player.

	A really huge question. Do GameEvents really need state? Well, it'd allow a bit of aid on synchronyzation, but at the same time it's a
	vector of attack. If events incpasulate action, then it is evident, that any state associated with this action should be outside
	of an action. Let's check some examples.

	Meele Weapon attack. So a player can attack with it's weapons. Slash with a sword, crush with a hammer and so on. It a simple attack it only affects
	the atacker and one target. It's reduces the target heath with respect of target's defencive cappablities(evasion, protection, armour) and attacker's
	offencive ones(strenght, mastery of a weapon, quality of a weapon). It also can inflict some debufs and it can affect the atacker like reducing weapon
	durability, wearing player's stamina or increasing it's rage level. In this situation there is clearly no actual state intrinsic to this event,
	but the problem is that there's migh be syncing problems, where player have changed it's weapon right before attacking, so when attack hits, it will...
	Oh wait. We use TCP/IP and there are other levels of anbstraction that together do guarantee, that events will have same ORDER on every simulation
	(with except of events, that are applayed localy before server confiramtion). So server's state definetly will be accurate, syncing problems
	might occur only with execution-before-confiramtion type of things. So if player changes weapons then attacks, and start executing attack
	event, but if server rejected change action upon recieving and then start performing action, it might result in desynchronyzation.
	One good way to resolve that is to completely prohibit execution of events before confirmation. We'll have about ~300ms of delays in the 
	resonable worst case scenario, but we can hide it with some animation and effects tricks.

	Digging example. A player digs a hole. Let's assume that all of our events are stateless and applied olny after confirmation. Well, that's
	fine. We send an event, server checks it and applies it, game state is changed so updates are sent out, which means, that player actually
	doesn't even have to apply events localy! But the stateless thing is a bit more complicated, because what if during our digging, something
	would happen to the ground, and right destroy a pice of ground we were digging. If we would assume, that we digging a block, that we're looking
	at, then we would suddenly destroy comletely diffrent blcok, and would find about it only when server sends us new state where bouth of the blocks
	are destroyed. It's not that bad and there's a pretty slim chance of rendomly stumbeling on this problem, but it's still not a very intutive
	behaviour. We could just remember which block we were looking at in the player's object itself that can be altered only by the player. We 
	definetly are going to validate it on usage but it would help us to check, if a thing we're about to do is realy what player intended.
	Two problems here is security and bad scalability. We now assume, that there is something that only player knows. But wait, the very creation
	of events is coming from the players! So it's not that extrodinare. But the scalability is a real issue. If for every such event we'd have
	to add aditional event that only tangentaly connected to the object itself, then it's just couldn't be done. But if it there's only a fiew
	actual parameters of this kind? So yeah, it looks like all that "What is it looking at" parameters are actually belong to the players, and
	not to a specific events. Which means, that events themselvs are stateles and can be wait for confirmation!

	Liftime of an event.
	Event is generated by the object producing a ticket along the way, that allow it to check on it's state.
	Now event is in EventController.
	Events controller performs basic checks to determine, if it's even worth it to send this event. Event is discarded if checks are failed.
	If checks are passed, event is geting deserialized and sent over to the server.
	It's important to remember, NetworkProcessor will actually store the event and is providing hooks
	on when command is confirmed or rejected. So we can just store some kind of pointer in command for clients
	mark sended events as "sended" and disallow their state change by anything but Processor when it ither "Rejects" or "Accepts"
	this event.

	But wait! Command queue is not intended to store commands for a long time! So this is a bad idea!
	Maybe we should just translate event state change? We can assign a unique number(Or ask Processor to do it for us).
	Then when event is just created and translated to the server. Server reconstructs it localy with association to
	session and unique-id of event. And imidately sends confirmation. Client meanwhile, when recives this confirmation, still
	assumes that event is not done. Or maybe some events states up front that they are fast and require only one confirmation?
	Well, for other events game's just started! Let's say it was a powerfull cast with a considerable cast-time. So player and server
	are waiting simultaniously for it performing quick cheks every frame if the time has come. During that moment, client or server
	might decide, that the cast is no longer viable, so they would send rejection for this event. Player never straight up uplies events,
	so if server rejects event, all what it means is that player can stop casting it. Things getting much more intense when player tries
	to stop event, that's allready was done on server's side. Whell, there is no rollback from that! But it means that client MUST wait for
	something like "Shure. Event is no more" or "Nope. Event is allready performed" from the server. Seems like a protocol to me. Now let's add
	some more examples.

	Client sends fast event. Such events are marked as FAST and require imidiate rejection/acceptence from the server.
	So EventsController marks it as SENDED and sends it with flag "FAST" to the server.
	Server recives this event and it must make decision during ApplyCommand of it's processor. So it immidately creates new event
	and processes it. Which might be not the best idea, b.t.w. but for now there is no clear disadvantages and it's withtin the current
	abilities of the system. So event is ither accepted or rejected and client recives the result.

	Now let's look ant regular events. An event is created and someone is going to wait for it. Probably displaing some animation or
	something like that, untill it's done or rejected. After initail checking this event recives status "processing". From this status
	it can ither become
	"stopping" - if client ask event to be terminated
	"rejected" - if server rejects the event during any moment after it is send, ither becouse of client's stop request or some inner mashination.
	"done" - if event is done. Note: event stopped event can become done if it's too late.
	So the client's can send info about one event up to two times. First on "processing" and then, if it is "stopping",
	btw there probably will be another state "stopping-sended" when event is stopping and this fact is translated to the server.
	
	So what would server do? Allright,, so we allready drop compile-time based solution. Or did we? We obvoiusly would love to have events,
	that may or may not store additional state and they may be a parameteraized object, or a class. This last thing is actually managable,
	if we allow	event's instances to change their ID on creation. Allright! Nexit thing is parametrization. What about ParameterBlock?
	It'll be another CRTP class with sizeof method. It's pretty simple in realization, but we must decide, ither to make this sizeof
	method constexpr, which would make things probably faster,
	or make this method virtual which would cause virtal table creation and loosing of POD-type status.
	But do we realy need POD? I think not. And the increase in capabilties os probably woth a bit of time and memory we will spend.
	We can event later redo this as constexpr-static!
	


	*/
}