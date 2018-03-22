#pragma once
#include "LandscapeChunk.h"
#include "LandscapeMeshGenerator.h"
#include <LinearMath\btVector3.h>
#include <cstdint>
#include <vector>
#include <memory>
#include <functional>
#include <map>
#include <algorithm>

namespace GEM::GameSim
{
	/**!
	More text. Becouse I have no idea, how to implement this.
	Let's start from the begining.

	Player connects to a server and requests chunks.
	Techicly speacking, server will know the position of a player at this moment, so it can predict it's request.
	But can it predict all the chunks, that player already have on a hard drive? Well, it probably can, but it's actually
	difficult, so let's say that it can't. In that case player's request will consist of vector of pairs[chunkVersion, ChunkPos].
	When server recives this messages, it should, for every specified chunk:
	1)Check that this chunk is visible for a player(Security check). If it's not, make warning and ignore this request
	2)Check version of chunk. If it's up to date, reply the same way player did. If it's not, prepare to send a chunk

	After every chunk is processed, server should form the responce. Responce, for every chunk, looks like this:
	[Version][Pos]. And at the end, there is [IsThereChunkPack]<ChunkPackData>.
	
	In the meantime, player should load required chunks from the disk, maybe even start showing them, but mark them as not-verified.
	It's assumed, that respond will be recived in a 0.6s. Then maybe we should not process chunks? Ok, so we do not process them then.

	When player recives server's responce, it should mark all the chunks, which version is equal to one in responce and then, if there is
	a chunkPack, it must unpack it and update corresponding chunks, also marking then as confirmed.

	And so chunks are now restored. For evry confirmed chunk, a mesh should be generated and then presented.
	Controller should generate meshes in a separate thread and when it's ready, it should mark which exact chunkMeshes were created.
	It's important to remember, that chunk, for which mesh is generated, shoud not be altered from any other place during that process.

	In this way, we can also provide std::optional<const Node*> GetClosestNode(pos); interface to read-only access to allready loaded,
	verified chunks.

	Next thing is alteration. It's a difficult thing, actually. When we alter a chunk, we create a temporary copy of a chunk(preferably 
	not the full one) of alterationg chunk with said alterations, and send an alterationRequest to a server. Request consists of
	sessionUnique ID and altered nodes with their positions. Request also, probably, should somehow reference a reason, why it was called.
	For example if player dug the ground, the alteration request should contain reference to that event, so that it validity can be checked
	by checking validity of that request. We can, probably, do this by injecting request in an event. Or even more, should this system
	be responsible for alterations at all? Well, it should provide temporary chunk facility and that's it. Then actual events should go with
	confirmations, and this confirmations, in turn, should validate or invalidate change in a chunk.

	One more thing about chunk loading. We can fetch all requested chunks, but we also should track chunks, that are not used anymore.
	We can give a ticket to every player, and every time player moves we also update position of a player in chunk system, and then check
	for the chunks we're need and for ones we're don't anymore.

	*/

	/**!
	Something about the size of a map.
	It's fixed. Map centered at 0,0 with MapSize/2-1 and 1MapSize/2 as the maximum and minimum coordinates.
	It's also a square.
	*/

	/**!
	And another part of text.
	After some thought, I came to a conclusion, that all we basically need is a way to determine, what chunks entered view in a frame
	what chunks left it and what chunks is visible for every loader.

	The most straightforward aproach would be to just iterate through every chunk in a map(we have a limited now, so it's doable, but
	slow) and test if it's visible for any Loader. this is an O(n*n*l) task with n - size of a map, l, number of loaders.
	Then we go through every chunk once again to determine, and compare it's visibility with visibility in a previous frame.
	Which is O(n*n) task.

	It's possible to make first task faster. We could iterate through every loader and just mark chunks, that is visible for it.
	Then on the second pass, chunk is count as visible, if at least one loader sees it.

	But can we somehow decrease the complexity of a second pass?
	What if for every loader we would just make a list of visible chunks and also store a list of previously visible ones.
	Then on the second pass we just concat all "visible" lists of all the loaders. Wait a little. We can, instead of building
	individual visibility list, build a huge one, and also sorted. We We can assign unique value for every chunk(x+size*y) and
	then, for every visible chunk of a loader, we simply try to add it in a map. We will succeed, if it's first time the chunk is visible by
	someone during this pass, and we will silently fail and continue, if it's allready added.
	Then we can compare this array with the one from previous tick. Elements that is in a new array, but not in the previous
	are just entered the view. Elements that is in a previous but not in a new, are just left the view. And during first
	part we also record every visible chunk on a per-loader basis, so we also have a chunks for every loader.
	But I just remembered, that we aslo need a NewChunks on a per-loader basis.
	New addition: some beautiful stranger in the internet provided a solutuin for "Find elements from two sorted arrays, that are
	unique for that arrays" problem that is O(m+n) in worst case.
	Ok, So that way we can find:
		Visible chunks per loader
		Newly visible chunks per loader

		Newly visible chunks.
		Chunks, that are no more visible.


	*/


	template<typename T>
	std::pair<std::vector<T>, std::vector<T>> FindDifferences(std::vector<T>& A1, std::vector<T>& A2)
	{
		int i = 0, j = 0;
		std::vector<T> A1Unique, A2Unique;

		while (i < A1.size() || j < A2.size())
		{
			if (j >= A2.size()) { A1Unique.push_back(A1[i]); i++; }
			else if (i >= A1.size()) { A2Unique.push_back(A2[j]); j++; }
			else
			{
				auto& el1 = A1[i];
				auto& el2 = A2[j];

				if (el1 != el2)
				{
					if (el1 < el2) {
						A1Unique.push_back(el1);
						i++;
					}
					else
					{
						A2Unique.push_back(el2);
						j++;
					}
				}
				else
				{
					if (i < A1.size()) { i++; }
					if (j < A2.size()) { j++; }
				}
			}
		}
		return std::make_pair(A1Unique, A2Unique);
	}

	class LandscapeChunkController
	{
	public:
		struct ChunkPos
		{
			int x;
			int z;

			ChunkPos(int _x, int _z) : x(_x), z(_z) {};

			static ChunkPos getChunkFromPoint(float x, float y, float z)
			{
				return ChunkPos(static_cast<int>(ceil(abs(x)) * (std::signbit(x) ? -1 : 1)) / 16 + (std::signbit(x) ? -1 : 0),
					static_cast<int>(ceil(abs(z)) * (std::signbit(z) ? -1 : 1)) / 16 + (std::signbit(z) ? -1 : 0));
			}
			static ChunkPos getChunkFromPoint(btVector3 vec)
			{
				return ChunkPos(static_cast<int>(ceil(abs(vec.x())) * (std::signbit(vec.x()) ? -1 : 1)) / 16 + (std::signbit(vec.x()) ? -1 : 0),
					static_cast<int>(ceil(abs(vec.z())) * (std::signbit(vec.z()) ? -1 : 1)) / 16 + (std::signbit(vec.z()) ? -1 : 0));
			}

			friend bool operator<(const ChunkPos& l, const ChunkPos& r)
			{
				return std::tie(l.x, l.z) < std::tie(r.x, r.z);
			}
			friend bool operator> (const ChunkPos& lhs, const ChunkPos& rhs) { return rhs < lhs; }
			friend bool operator<=(const ChunkPos& lhs, const ChunkPos& rhs) { return !(lhs > rhs); }
			friend bool operator>=(const ChunkPos& lhs, const ChunkPos& rhs) { return !(lhs < rhs); }

			friend bool operator==(const ChunkPos& lhs, const ChunkPos& rhs) {
				return std::tie(lhs.x, lhs.z) == std::tie(rhs.x, rhs.z);
			}
			friend bool operator!=(const ChunkPos& lhs, const ChunkPos& rhs) { return !(lhs == rhs); }
		};

		struct LoaderType
		{
			using LoaderIDType = uint64_t;
		private:
			static LoaderIDType LastLoaderUniqueID;
		public:
			std::function<btVector3()> posFunc;
			std::vector<ChunkPos> visbleChunks;
			std::vector<ChunkPos> newlyVisibleChunks;

			LoaderIDType loaderUniqueID;

			LoaderType(std::function<btVector3()> _posFunc) : loaderUniqueID(LastLoaderUniqueID++), posFunc(_posFunc) {};
		};

	private:

		std::vector<ChunkPos> m_globalyVisibleChunks;
		std::vector<ChunkPos> m_noLongerVisibleChunks;
		std::vector<ChunkPos> m_newlyGlobalyVisibleChunks;

		std::vector<LoaderType> m_loaders;

		int m_loadRadius;
	public:

		LandscapeChunkController() : m_loadRadius(3) {};

		inline void setloadRadius(unsigned int newVal) { m_loadRadius = newVal; }

		inline LoaderType::LoaderIDType createNewLoader(std::function<btVector3()> PosFunc)
		{
			m_loaders.emplace_back(PosFunc);
			return m_loaders.back().loaderUniqueID;
		}

		inline bool RemoveLoader(LoaderType::LoaderIDType loaderID)
		{
			auto Loader = std::find_if(m_loaders.begin(), m_loaders.end(), [&](LoaderType& loader) {return loader.loaderUniqueID == loaderID; });
			if (Loader == m_loaders.end()) { return false; }
			m_loaders.erase(Loader);
			return true;
		}

		void ProcessChunks();
	};

}