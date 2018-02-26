#pragma once
#include "EventBase.h"
#include <vmmlib/vector.hpp>
#include <vmmlib/math.hpp>

/**!
NumerableEvents. So there are some events, that have a unique(for a given session of a given client) ID.
They works kind of like a requests. For example, if client perform a movement, it sends a NumerableEvent, that incapsulate
it's new position, and as soon as it send it, it assumes, that server have accepted this, and so it's ignoring some of the synchronization
events for that entity, reasoning that it's actual state is actually correctly depicted on a client, and server just don't recive
all the other messages to come to the same state. And it's messages are also lagging behind.
Meanwhile server recives all the events, that could affect the state of this entity of interest in order with this NumerableEvents(Meaning that no regular event,
that came were generated after a NumerableEvent, could be processed before that event. And All the events, that were generated before a NumerableEvent,
must also be processed before it). In that case server could confirm this NumerableEvents, accepting their states to an entity of interest. It still
would generate an Update for that entity which probably, still be ignored by a client. But if Server determins, that NumerableEvent contains an
incorrect state, it does not apply it's state. Instead it wanrs a client, sending it an OutOfSync event(or something)
that contain ID of that NumerableEvent.
It's all good and all, but it's expected, that if state of one NumerableEvent is declined, that some or all of the following NumerableEvents, state of
which is a product of a declined state in some way, will also be incorrect. Then what should the player do?

For example Player1 tries to move front, by as soon as it starts, another Player2 builds a wall right in front of it.
So Player1 starts moving, and also sending PlayerMoveForward event, and NumerableEvent_Movable envents along the way.
Server recived a PlayerMoveForward event and some of the NumerableEvent_Movable events from Player1. It also recived
BuildWall(or something) from Player2 in a same time. It's assumes, that Player2 built a wall before Player1 started moving, but only some
NumerableEvent_Movable were rejected. But Player1 were actually shifted a little to the left as it moved, so now it's OutOfSync,
but still doesn't know about it.
Server continues to reject all client's new NumerableEvents and that happens untill Player1 recives OutOfSync. As soon as it did it, it will Update
it's position accordingly, and will continue to move forward with that shift. But Server still don't know it, it continues to recive incorrect
NumerableEvents and preparing new OutOfSync events. As soon as it would reach Player1, it will once again teleport Player1 to the place, where
it shifted at first time(becouse at the moment of Server sending this event, no new correct NumerableEvents from that player where processed).
And then it will basically repeat it's path, sending new NumerableEvents along the way. At this point Server starts to recive NumerableEvents
from the moments, where Player1 just started shifting from the wall. And it's all OK, unitll Server starts reciving NumerableEvents from the moment
where Client where teleported back. This is incorrect state! So this events will be rejected and all that stuff will repeat itself.

In time of writing 2 ideas emerged.
1) NumerableEvents should continue not a difference of states, not a state.
2) Client should keep track of sended events, untill it recives a confirmation for them.
Let's apply that and see if option 1 as actually needed.

Player1 tries to move forward, but Player2 builds a wall right on it's path. Server recives BuildWall event, PlayerMoveForward and a bunch
of NumerableEvents about Player1 movements. Server rejects some of this event at the end, shifting the player along the wall instead.
During that time Player1 continues to move forward, oblivious about the wall, that just appeared on it's path. Then Server packets are recived.
Client marks declined events, forgets about them, applying a state from Update to it's position, tries to re-apply it's remaining NumerableEvents
that it allready sent to a Server. Determines that they can't be applied(Becouse they where moving forward, and now player can only move along the wall).
During that Player marks them as potentialy declined and instead starts moving along the wall and sending events about that. Server recives
this new batch of events, that it also declines, sending OutOfSync packet and correctly updating state of the Player1. Player1 still sliding along the
wall, recives this packet from the Server, determines that all the OutOfSync events were allready marked as potentially declined, and just forgets about them.
This time this OutOfSync wasn't a surprise for a Client, so it's assumes, that from it's state it's state is actually correct this time, so it does nothing.
Then server recives new SlidingTheWall events, determines that they are correct and applies them.

So it looks good. Server and client successfully re-Sync'd. But it didn't sould like there are actual profit from sending only the diffrence, maybe a
space-wise at best.
So now let's try to brake it all somehow.

Another day have passed and now it's clear, that NumerableEvent should ither contain full state or shouldn't contain any state at all(Just making
PlayerInput) would do the trick. Or would it? Let's see. Now PlayerInput events are numerable. So when OutOfSync comes, it can send OutOfSync,
that would signify the Entity, which Events where altered and then ID of the NumerableEvents, that should be corrected. Then when this packet comes
Player could just re-apply this events. But right now PlayerInput have only two-state events associated with it. So we need some event
that would repeat every frame, signifing that player are actually moving. And only this events can be numerable.

So OK, every frame we send Numerable "Movement" events. Now only 2 problems left.
1)Server ticks and Player ticks are not equal. So if player said that it moved for one frame left, the difference in actual movement can be up to 1000%
2)Events from player can lag behind actual simulation. For example if player stopped moving, we could recive this events only a 0.2 seconds after wheere
it actually should stop. Now when you think of it in terms of seconds, not milliseconds, it doesn't sound that bad, actually. I mean, 0.2 seconds. Come on!
How far it can be? But even if it's not that far, after 0.35 seconds from when the movement have stopped, Player would recive a correctional Update,
that would shift it. Once again, it 0.2 seconds. 20 frames in a good day, 10 in a bad one. Even more, you allways can interpolate this correction on a client
for a couple dozens of frames, making this shift smooth as silk!

But still, we need a way to determine the time. So idea: Server allready sends it's time with every packet. 
This will allow Server to determine, in case it it's needed, which event is actually first. But can we trust it?
OK, so we're not just accept any time signature. It MUST be a time AFTER the last recived time singanture for this player
and before the beginning of the Server Tick. So it' can't travel from the past or from the future. Ok, let's imagine a player
allways sending one packet with overwelming time signatures. As big, as it is possible within those rules. (Player wants to use it to
move faster, or be the first one in something). How far can Player go with that? Well, not farther then the ping/2. Player
could try to tame the wave and stay right on the edge, but one sudden downspike in a ping would immediately make Player's ill intentions
known to a server. If player would prefer to stay a little farther from the edge it still can gain advantage though. Or can it?
Well yes, it seems like it can. About 0.05s of advantage. And to do that, Player must monitor it's connection latency constantly.
So the decision is that it doesn't worth it.
*/

namespace GEM::GameSim
{
	using NUMEVENT_ID_TYPE = uint32_t;
	class NumerableEvent : public EventBase
	{
	public:
		//Holds the temporary ID for this event.
		const NUMEVENT_ID_TYPE eventID;
		NumerableEvent(NUMEVENT_ID_TYPE Nid) : eventID(Nid) {}
	};

	class NEvent_Movement : public NumerableEvent
	{
		GAMESIM_EVENT(x)
	public:
		vmml::vec3f newPos;

		
		NEvent_Movement(NUMEVENT_ID_TYPE id, vmml::vec3f pos) : NumerableEvent(id), newPos(pos)
		{}
		inline static std::unique_ptr<EventBase> deserialize(cereal::BinaryInputArchive& ar)
		{
			NUMEVENT_ID_TYPE Nid = 0;
			vmml::vec3f vec;
			ar(Nid, vec.x(), vec.y(), vec.z());
			return std::make_unique<NEvent_Movement>(Nid, vec); 
		}
			virtual void serialize(cereal::BinaryOutputArchive& ar) const override { ar(eventID, newPos.x(), newPos.y(), newPos.z()); }
	};
}