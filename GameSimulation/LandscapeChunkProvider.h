#pragma once
#include "LandscapeChunkPack.h"
#include <cereal\cereal.hpp>
#include <cereal\archives\binary.hpp>
#include <string>
#include <sstream>
#include <optional>

/**!
Ok. So, chunks.
Last time i've tackled this problem, chunk updates was the main thing.
So let's start with them.
Actually, no. First, we need to get to them.
So once again, the beginning.

1) Client knows nothing and just connects to a server
Client's position become known to a server before it does for a client. So server may 
start to prepare chunks, that are needed. This means ither to load them up and unpack them
or to generate them from scratch.
In the meantime client would recive it's position and will attempt to load a map. Client's
map loader should attempt to fetch map parts from the disk, but as we stated before, there is nothing right now,
so client must skip map building for now and, if chunk that contains the player can't be build also, simulation must
be suspended.
But wait. If server just sends all the chunks ASAP, then it will allways send all the chunks, even then client allready
have a newest version of some of them. Which is pretty rare condition, actually. So should server just update
client's map when it feels like it, or should client make requests for every update?
Let's just go with the frirst option for now, becouse it's easier.
All right, so client just recived map chunks data. Then it's going to apply it. As soon as chunk data is avaliable,
client should start building chunks. And as soon as chunk where the player is is built, simulation should be unsuspended.
Seems good for now.

2) Client knows old data and connects to a server
Once again, server determines player position and sends required chunk. Or it waits till player sends a request and then 
send only packs, that are obsolite on the client.
During that time, client fetches the data from the disk and build chunks from them, but they all marked as unconfirmed.
This means that they should not be modified, and may change in any second. Maybe even while they are being built.
In that case client should somehow queue build chunk tasks, so that can be dropped as soon as there is a newer version of this chunk.
During that build time, new chunk data arives, and it's freshier, that the current one being built.
So server should stop building. Mark every chunk, which version is considered up-to-date as confirmed, and replace all the unconfirmed ones,
also confirming them after that. And, well, build everithing, that is still not built.

3) Client allready connected and changes a chunk.
Change in a chunk is determined by position of a chunk, position of a node being changed and it's new state.
One ChunkChanges contains several of such changes, just to simplify the process.
There was an idea, that player should not be able to directly alter a chunk. Instead it should send some event to a server
which, in turn, would alter that chunk. Then server transmits this update to everyone interested in it, including a sender, which
can use it to verify a change. But this means, that player would see updates it's making with a delay, which is bad.
Even worse, we get a verefication only when it's accepted, but what if it's not? I guess events should have a cofirmation by itself then, so
that when an event is aborted, we would know that. But wait, it actually have that functionality allready! Clients track a history
of all events being sent to a server and server can send OutOfSync messages to determine, that an event wasn't accepted! So if our
terrain-updating event was rejected, we will know that. But there's still a problem. What should we do, while an event is not
confirmed nor denied? Should we update a client's terrain to negate a delay. Let's say we will. In that case, terrain should have some
"undo" functionality. Even worse, actually. History system does not call "Undo" on events that been rejected. It just drops them. I guess
what we should do, is to keep last confirmed state of a chunk and then just make a new unconfirmed one for all the changes, that is still unconfirmed.
Then as soon as that changes were confimred, which we're also have no way to find out, we should apply all the confirmed changes instead.
If at least one change was rejected, we should...

*/

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

*/

class SynchroEventBase
{
	/**!
	Determines unique value for an event, so that it could be easily tracked by other systems
	*/
	uint64_t m_uniqueEventID;
	
	/**!
	If event is in play, somehow, but it's not confirmed by a server, this method will
	be called. Event must be performed, but user should be prepared to revert
	this event, if server refuses to apply this event.
	If event can't be applied, false must be returned. If event applied successfuly, return true.
	This method will be called only once for every event
	*/
	virtual bool ApplyEventUnconfirmed()=0;

	/**!
	Every event is stored in history. If something changes in history, every event in it
	will be teared down and then reapplied. Re-application can happen multiple times
	for one entitiy, if history is unstable enough. This method will never be called for a confirmed
	event. It is assumed, that when this method is called, gameState will be the same every time
	and it will be just like when ApplyEventUnconfirmed was called, or some previous
	events was discarded by the server.
	True must be returned, if event was applied successfuly. False if event cannot be re-applied.
	*/
	virtual bool ReapplyEventUnconfirmed() {};

	/**!
	Events are stored in history. If some event in a history was discarded by the server, or something
	else had jeopardized validity of a history, every event in it, up to the most recent valid ones
	will be reverted in order from most reacent ones to the oldest ones, and then events that wasn't rejected
	will be put back in the same order they where put in in the first time.
	This method should return a gameState to where it was, before this event was applied.

	\note. Events are different. Some events store only partial information, and for them
	there is a difference, in which state the system in right now, becuse result of applyment of
	an event depends on a current state event's subject is in right now.
	But there is also events that contains full state of subjects. Such events doesn't need
	this revert mechanism, becouse for them, result of applying en event doesn't depend on
	a current state of a subject
	*/
	virtual bool RevertEventUnconfirmed() {};

	/**!
	If an event was rejected by the server, history will throw it off and call this method.
	If system, that uses events wants some mean of knowing, when event was canceled, here it is.
	End yeah, btw, this is one of two final steps in event's liftime. It either get's confirmed,
	or it get's rejected. So no calls will be made after this one
	*/
	virtual void RejectEvent() {};

	/**!
	Events in history are held till server confirms them. This method is called for an event right
	when it get's confirmed and leaves history. This is one of the final steps in event's lifetime
	so no calls will be made after this one.
	*/
	virtual void ConfirmEvent() {};

};

namespace GEM::GameSim
{
	class LandscapeChunkProvider
	{
	private:
	public:
		
		/**!
		Starts the process of retreaving chunk, that wasn't loaded before.
		Retreave can take some time, so to avoid freezing it, this method does
		not guarantee, that chunk will be loaded right after it's execution end.
		*/
		virtual void RequestChunk(int x, int z)=0;

		/**!
		Returns true, if chunk is loaded and ready to be modified, processed or something else.
		*/
		virtual bool isChunkReady(int x, int y)=0;
		
		/**!
		Removes chunk from memory, so it would no longer occupy space
		*/
		virtual void UnloadChunk(int x, int y) = 0;
	};

	class TestChunkStuff_Server
	{
		struct ChunkHeader
		{
			uint64_t version;
			int32_t posX, posZ;

			template<class Archive>
			void serialize(Archive & archive)
			{
				archive(posX, posZ, version);
			}
		};
		struct RequestForm
		{
			std::vector<ChunkHeader> RequestedChunks;
		};

		struct ResponceForm
		{
			std::vector<ChunkHeader> chunkHeaders;
			LandscapeChunkPack PackedChunks;
		};

	public:

		bool isChunkAllowed(const ChunkHeader form)
		{
			//Right now there is no way to check, if chunk is allowed or not
			return true;
		}
		bool isChunkUpToDate(const ChunkHeader form)
		{
			//Right now chunks are allways up to date
			return true;
		}
		ChunkHeader getUpToDateChunkHeader(const ChunkHeader form)
		{
			return ChunkHeader(form);
		}
		const LandscapeChunk* getChunkPtr(const ChunkHeader form) { return nullptr; }

		/**!
		Recives request from a player and repares a responce.
		Request contains chunk posiontion and version, that player possesses.
		Responce contains most recent versions of requested chunks and, if needed, a pack of all missing chunks.
		*/
		ResponceForm ReciveRequest(const RequestForm RequestedChunks)
		{
			ResponceForm result;
			std::vector<const LandscapeChunk*> ChunksThatShouldBeSent;

			for (auto& chunkrequest : RequestedChunks.RequestedChunks)
			{
				if (isChunkAllowed(chunkrequest))
				{
					if(isChunkUpToDate(chunkrequest))
					{//If chunk is allowed and up to date, just add it's info in responce
						result.chunkHeaders.push_back(chunkrequest);
					}
					else
					{
						auto UpToDateHeader = getUpToDateChunkHeader(chunkrequest);
						result.chunkHeaders.push_back(UpToDateHeader);
						getChunkPtr(UpToDateHeader);
					}
				}
				else
				{
					//Ignore request. Warn the server.
				}
			}

			result.PackedChunks = LandscapeChunkPack(ChunksThatShouldBeSent);
		}

		/**!
		Prepares an update, if it's needed.
		\param[in] PlayersCurrentStatus contains info about versions of a chunk, that player should currently have
		\returns returns a list of chunks that should be updated and a chunkPack, that contains newest versions of this chunks.
		*/ 
		std::optional<ResponceForm> PrepareUpdate(std::vector<ChunkHeader> PlayersCurrentStatus)
	};
}