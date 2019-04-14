#pragma once
#include "LandscapeChunk.h"
#include <LinearMath\btVector3.h>
#include <cstdint>
#include <algorithm>

namespace GEM::GameSim
{
	struct VertexType
	{
		btVector3 pos;
		btVector3 normal;
		LandscapeNode* OriginalNode1;
		LandscapeNode* OriginalNode2;

		VertexType(btVector3 _pos, LandscapeNode* Node1, LandscapeNode* Node2) : pos(_pos), normal((btScalar)0, (btScalar)0, (btScalar)0), OriginalNode1(Node1), OriginalNode2(Node2) {}
	};

	/**!
	It's really hard to texture a 3d object with a 2d texture. So we keep track of all the
	created triangles during the mesh creation with the normal a particular triangle is having
	so that visualizator could do something with it.
	*/
	struct triangleType
	{
		uint32_t indices[3];
		btVector3 normal;
	};

	struct LandscapeMesh
	{
		std::vector<VertexType> m_vertices;
		std::vector<uint32_t> m_indices;
		std::vector<triangleType> m_triangles;
	};

	/**!
	Generates vertex mesh from provided LandscapeChunk and holds it!
	*/
	class LandscapeMeshGenerator
	{
	private:

		struct CellRepresentation
		{
			/*First part is the actual vertex, second is it's position in Vertices array. Max value of uint32_t is considered as not-set
			and is used to actually check, if certain vertex allready where created, so avoid meshes with stupidly huge(2^32) ammount of indices!
			*/
			uint32_t Vertex[3] = { (std::numeric_limits<uint32_t>::max)(),
				(std::numeric_limits<uint32_t>::max)(),
				(std::numeric_limits<uint32_t>::max)() };

			/**!
			Used for calculating normals on edges withous creating vertices, that should not be visible
			*/
			struct PerVertex
			{
				btVector3 pos;
				btVector3 normal;
			};

			PerVertex data[3];

		};

		/**!
		A bag of information, that is used during generation of a mesh
		*/
		struct ProcessingData
		{
			LandscapeChunk* m_chunkCenter;
			LandscapeChunk* m_chunkForward;
			LandscapeChunk* m_chunkRight;
			LandscapeChunk* m_chunkForwardRight;

			

			//Holds last last layer of processed nodes and a current one
			CellRepresentation NodeDecks[2][LandscapeChunk_Size + 3][LandscapeChunk_Size + 3];

			LandscapeMesh tmpMesh;
		};

		/**!
		Returns the value of a node from four providing chunks in such a way that, assuming LandscapeChunk_Size = 16:
		x:[-1,15] y[any] z[-1,15] are taken from the center chunk.
		x:[-1,15] y[any] z[16,18] are from ForwardChunk.
		x:[16,18] y[any] z[-1,15] are from RightChunk.
		x:[16,18] y[any] z[16,18] are from ForwardRightChunk.

		\note This layout abstracts chunk differance for the rest of marching cubes algorithm.
		*/
		static inline uint8_t getNodeValue(int x, int y, int z, ProcessingData& tmpData)
		{
			if (y < 0) { return 0; }
			if (x < static_cast<int>(LandscapeChunk_Size-1))
			{
				if (z < static_cast<int>(LandscapeChunk_Size-1))
				{
					return tmpData.m_chunkCenter->getNodeValue (x + 1, y, z + 1);
				}
				else
				{
					return tmpData.m_chunkForward->getNodeValue(x + 1, y, z + 1 - LandscapeChunk_Size);
				}
			}
			else
			{
				if (z < static_cast<int>(LandscapeChunk_Size-1))
				{
					return tmpData.m_chunkRight->getNodeValue(x + 1 - LandscapeChunk_Size, y, z + 1);
				}
				else
				{
					return tmpData.m_chunkForwardRight->getNodeValue(x + 1 - LandscapeChunk_Size, y, z + 1 - LandscapeChunk_Size);
				}
			}
		};

		/**
		Returns a node from chunks quadrant with a shift of -1 so that Marching Cubes could work with less chunks at a time
		\warn Values if y < 0 will be clamped to 0
		*/
		static inline LandscapeNode& getNode(int x, int y, int z, ProcessingData& tmpData)
		{
			if (y < 0) { y = 0; }
			if (x < static_cast<int>(LandscapeChunk_Size - 1))
			{
				if (z < static_cast<int>(LandscapeChunk_Size - 1))
				{
					return tmpData.m_chunkCenter->getNode(x + 1, y, z + 1);
				}
				else
				{
					return tmpData.m_chunkForward->getNode(x + 1, y, z + 1 - LandscapeChunk_Size);
				}
			}
			else
			{
				if (z < static_cast<int>(LandscapeChunk_Size - 1))
				{
					return tmpData.m_chunkRight->getNode(x + 1 - LandscapeChunk_Size, y, z + 1);
				}
				else
				{
					return tmpData.m_chunkForwardRight->getNode(x + 1 - LandscapeChunk_Size, y, z + 1 - LandscapeChunk_Size);
				}
			}
		};

		/**!
		Processes one cube of a mesh
		*/
		static void ProcessOneCube(int x, int y, int z, bool RegisterNewVertices, ProcessingData& tmpData);


	public:
	
		static LandscapeMesh Generate(LandscapeChunk* ChunkCenter, LandscapeChunk* ChunkForward, LandscapeChunk* ChunkRight, LandscapeChunk* ChunkForwardRight);

	};
}

/**!
Once again, second implementation of a marching cubes! 
*/