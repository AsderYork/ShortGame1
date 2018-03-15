#pragma once
#include "LandscapeChunk.h"
#include <LinearMath\btVector3.h>
#include <cstdint>
#include <algorithm>

namespace GEM::GameSim
{
	/**!
	Generates vertex mesh from provided LandscapeChunk
	*/
	class LandscapeMeshGenerator
	{
	private:
		std::vector<btVector3> m_vertices;
		std::vector<uint32_t> m_indices;

		const LandscapeChunk* m_chunkCenter;
		const LandscapeChunk* m_chunkForward;
		const LandscapeChunk* m_chunkRight;
		const LandscapeChunk* m_chunkForwardRight;

		const int32_t m_chunkPosX;
		const int32_t m_chunkPosZ;

		struct CellRepresentation
		{
			/*First part is the actual vertex, second is it's position in Vertices array. Max value of uint32_t is considered as not-set
			and is used to actually check, if certain vertex allready where created, so avoid meshes with stupidly huge(2^32) ammount of indices!
			*/
			uint32_t Vertex[3] = {(std::numeric_limits<uint32_t>::max)(),
				(std::numeric_limits<uint32_t>::max)(),
				(std::numeric_limits<uint32_t>::max)()};
		};

		//Holds last last layer of processed nodes and a current one
		CellRepresentation NodeDecks[2][LandscapeChunk_Size + 1][LandscapeChunk_Size + 1];


		/**!
		Returns the value of a node from four providing chunks in such a way that, assuming LandscapeChunk_Size = 16:
		x:[-1,15] y[any] z[-1,15] are taken from the center chunk.
		x:[-1,15] y[any] z[16,18] are from ForwardChunk.
		x:[16,18] y[any] z[-1,15] are from RightChunk.
		x:[16,18] y[any] z[16,18] are from ForwardRightChunk.

		\note This layout abstracts chunk differance for the rest of marching cubes algorithm.
		*/
		inline uint8_t getNodeValue(int x, int y, int z)
		{
			if (y < 0) { return 0; }
			if (x < static_cast<int>(LandscapeChunk_Size-1))
			{
				if (z < static_cast<int>(LandscapeChunk_Size-1))
				{
					return m_chunkCenter->getNodeValue (x + 1, y, z + 1);
				}
				else
				{
					return m_chunkForward->getNodeValue(x + 1, y, z + 1 - LandscapeChunk_Size);
				}
			}
			else
			{
				if (z < static_cast<int>(LandscapeChunk_Size-1))
				{
					return m_chunkRight->getNodeValue(x + 1 - LandscapeChunk_Size, y, z + 1);
				}
				else
				{
					return m_chunkForwardRight->getNodeValue(x + 1 - LandscapeChunk_Size, y, z + 1 - LandscapeChunk_Size);
				}
			}
		};

		/**!
		Processes one cube of a mesh
		*/
		void ProcessOneCube(int x, int y, int z);

	public:
		LandscapeMeshGenerator(const LandscapeChunk* ChunkCenter, const LandscapeChunk* ChunkForward, const LandscapeChunk* ChunkRight, const LandscapeChunk* ChunkForwardRight);
	
		/**!
		Returns position of a chunk
		\returns <x,z> position of a chunk
		*/
		inline std::pair<int32_t, int32_t> getPos() const { return std::make_pair(m_chunkPosX, m_chunkPosZ); }

		inline const std::vector<btVector3>& getVertices() const { return m_vertices; }
		inline const std::vector<uint32_t>& getInidces() const { return m_indices; }
	
	};
}

/**!
Once again, second implementation of a marching cubes! 
*/