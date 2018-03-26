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
		//std::optional<ResponceForm> PrepareUpdate(std::vector<ChunkHeader> PlayersCurrentStatus) {};
	};
}