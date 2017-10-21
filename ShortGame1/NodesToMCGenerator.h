#pragma once
#include "NodeChunk.h"

namespace GEM
{
	/**!
	Base class for NodesToMCGenerators
	*/
	class NTMCG_Base
	{
	protected:
		//DimXZ - Dimensions X and Z of a NodeChunk. Just in case if they would change. DimY is the same, but for Y		
		int DimXZ, DimY;

		int m_chunkPosX, m_chunkPosZ;
	public:

		NTMCG_Base(int _dimXZ, int _dimY, int _chunkPosX, int _chunkPosZ) :
			DimXZ(_dimXZ),
			DimY(_dimY),
			m_chunkPosX(_chunkPosX),
			m_chunkPosZ(_chunkPosZ)
		{}

		/**!
		Creates MC anew. It clears all buffers and starts from the beginning.
		*/
		virtual void Generate() =0;

		/**!
		Updates Index and Vertex to match current NodeMap.
		It assumes, that Generate were allready called at least once
		*/
		virtual void Update() =0;



		struct MidPointBase
		{
			float x=0, y=0, z=0;
			float nx = 0, ny = 1, nz = 0;

			//Texture cordinate are actually 3d. But with a help of a flavors ony 2 of cordinate will end in actual mesh
			float uvx = 0, uvy = 0, uvz = 0;
			//This also means that MCToMesh should be able to work differetiate flavors correctly, choosing right u, v pair for a given flavor.
			enum Flavor { FLAVOR_UPDOWN, FLAVOR_LEFTRIGHT, FLAVOR_FRONTBACK } flavor;
		};

		int const getChunkX();
		int const getChunkZ();

		virtual MidPointBase* getVertexVectorElement(int i) = 0;
		virtual const int getVertexVectorSize() = 0;

		virtual int getIndexVectorElement(int i) = 0;
		virtual const int getIndexVectorSize() = 0;
	};

	/**!
	Holds all needed data to generate one MarchingCube Nesh for one chunk.

	The idea is that GeneratorController will create Generator for every chunk that should be created, provide them with all needed data
	and then lets them generate meshes
	*/
	class NodesToMCGenerator : public NTMCG_Base
	{
	private:
		//A chunks. From them a mesh will be generated
		std::shared_ptr<GEM::NodeChunk> ChunkCentre, ChunkRight, ChunkFront, ChunkFrontRight;

		

		/**!
		We actually wants to find a points on edges of cubes, that would be a part of iso-surface.
		MidPoint is a representation of those points. It's just a float triplet of (x,y,z)
		MidPoint counts as Unset if X or Y or Z is -1; That's their default state
		*/
		struct MidPoint : public NTMCG_Base::MidPointBase {			
			/**
			Every flavor points to a position in a VertexArray where this particular midpoint were used as UpDown, LeftRight or FrontBack vertex.
			That's why VertexList MUST have no more then 3 occurences of a single Vertex. And every accurance must be flavored uniquely so that there shouldn't
			be two UpDown or something like that.

			This also means that MCToMesh should be able to work differetiate flavors correctly, choosing right u,v pair for a given flavor.
			*/
			int FlavorUpDown = -1;
			int FlavorLeftRight = -1;
			int FlavorFrontBack = -1;			
		};

		/**!
		Nodes that comes from MapGenerator contains only things, specific to that node. But we also need some way to access mid-points, associated
		with given node. So we just do an envelope that includes that midpoints.
		According to a paper about MarchingCubes, Midpoints that are represented here lays on a 0,3,8
		MidPoints should be set to a value, that is weighted average of nodes on their edge, assuming that node [x][y][z] have cordinates(x;y;z)
		*/
		struct NodeEnvelope {
			MidPoint Right, Back, Top;
			bool isChanged = false;
		};


		std::vector<std::vector<std::vector<NodeEnvelope>>> NodeEnvelopeVec;
		//This fector is flat. It must be accessed in the order values have been added in it
		std::vector<unsigned char> m_cubeValuesCash;

		//Holds pointers to used EdgePoints in order and without duplications
		std::vector<MidPoint*> VertexVector;
		//Holds IDs from VertexVector, so that they could be arranged in triangles
		std::vector<int> IndexVector;

		/**!
		Acesses node from multiple NodeMap as they are one big NodeMap.
		\param[in] NodeX Position of a node in a big NodeMap
		\param[in] NodeY Position of a node in a big NodeMap
		\param[in] NodeZ Position of a node in a big NodeMap

		\note This method have really narrow contract. It assumes, that user insert correct data and does not perform any checks.
		It also actively uses data of instance that it's called from, so...
		NodeX MUST be between 0 and DimXZ+1;
		NodeZ MUST be between 0 and DimXZ+1;
		NodeY MUST be between 0 and DimY;
		*/
		inline Node& GetNode(int NodeX, int NodeY, int NodeZ)
		{
			if ((NodeX >= DimXZ))
			{
				if ((NodeZ >= DimXZ))
				{
					return ChunkFrontRight->NodeMap[NodeX - DimXZ][NodeY][NodeZ - DimXZ];
				}
				else
				{
					return ChunkRight->NodeMap[NodeX - DimXZ][NodeY][NodeZ];
				}
			}
			else if (NodeZ >= DimXZ)
			{
				return ChunkFront->NodeMap[NodeX][NodeY][NodeZ - DimXZ];
			}
			else
			{
				return ChunkCentre->NodeMap[NodeX][NodeY][NodeZ];
			}
		}

		/**!
		Access Nodes as they are in a Cubie.
		\param[in] CubeX X-cordinate of a cubie
		\param[in] CubeY Y-cordinate of a cubie
		\param[in] CubeZ Z-cordinate of a cubie
		\param[in] NodeID Id of a node in a cubie, according to a MarchingCubes paper
		*/
		inline Node& GetNodeAsCubie(int CubeX, int CubeY, int CubeZ, int NodeID)
		{
			switch (NodeID)
			{
			case 0: return GetNode(CubeX, CubeY, CubeZ);
			case 1: return GetNode(CubeX + 1, CubeY, CubeZ);
			case 2: return GetNode(CubeX + 1, CubeY, CubeZ + 1);
			case 3: return GetNode(CubeX, CubeY, CubeZ + 1);

			case 4: return GetNode(CubeX, CubeY + 1, CubeZ);
			case 5: return GetNode(CubeX + 1, CubeY + 1, CubeZ);
			case 6: return GetNode(CubeX + 1, CubeY + 1, CubeZ + 1);
			case 7: return GetNode(CubeX, CubeY + 1, CubeZ + 1);
			}
		}

		/**!
		Finds a value of a cube, according to a paper
		*/
		inline int GetCubieValue(int CubeX, int CubeY, int CubeZ)
		{
			int cubeindex = 0;
			if (GetNodeAsCubie(CubeX, CubeY, CubeZ, 0).Value > 0) cubeindex |= 1;
			if (GetNodeAsCubie(CubeX, CubeY, CubeZ, 1).Value > 0) cubeindex |= 2;
			if (GetNodeAsCubie(CubeX, CubeY, CubeZ, 2).Value > 0) cubeindex |= 4;
			if (GetNodeAsCubie(CubeX, CubeY, CubeZ, 3).Value > 0) cubeindex |= 8;
			if (GetNodeAsCubie(CubeX, CubeY, CubeZ, 4).Value > 0) cubeindex |= 16;
			if (GetNodeAsCubie(CubeX, CubeY, CubeZ, 5).Value > 0) cubeindex |= 32;
			if (GetNodeAsCubie(CubeX, CubeY, CubeZ, 6).Value > 0) cubeindex |= 64;
			if (GetNodeAsCubie(CubeX, CubeY, CubeZ, 7).Value > 0) cubeindex |= 128;
			return cubeindex;
		}

		/**!
		Calculates MidPoint on a specified Edge
		\param[in] CubeX Cordinate of a cube
		\param[in] CubeY Cordinate of a cube
		\param[in] CubeZ Cordinate of a cube
		\param[in] EdgeID ID of edge, which MidPoint must be calculated
		*/
		MidPoint& CalcMidPoint(int CubeX, int CubeY, int CubeZ, int EdgeID);

		/**!
		Processes once Cube. Calculates its value, sets midpoints, creates triangles
		This method should be called only once for every call to Generate()
		*/
		void CreateCube(int CubeX, int CubeY, int CubeZ);

		/**!
		Updates once Cube. Calculates its value, sets midpoints, creates triangles
		This method should be called after CreateCube, untill Generate get's called?
		*/
		void UpdateCube(int CubeX, int CubeY, int CubeZ);

		/**!
		Creates new envelope.
		Assuming that it's empty
		*/
		inline void regenerateEnvelope()
		{
			NodeEnvelopeVec.resize(DimXZ + 1);
			for (int x = 0; x < DimXZ + 1; x++)
			{
				NodeEnvelopeVec[x].resize(DimY);
				for (int y = 0; y < DimY; y++)
				{
					NodeEnvelopeVec[x][y].resize(DimXZ + 1);
				}
			}
		}

		int m_chunkPosX, m_chunkPosZ;

	public:
		NodesToMCGenerator(std::shared_ptr<GEM::NodeChunk> _ChunkCentre, std::shared_ptr<GEM::NodeChunk> _ChunkRight, std::shared_ptr<GEM::NodeChunk> _ChunkFront, std::shared_ptr<GEM::NodeChunk> _ChunkCentreFront, int _DimXZ, int _DimY, int ChunkX, int ChunkZ)
			:
			ChunkCentre(_ChunkCentre),
			ChunkRight(_ChunkRight),
			ChunkFront(_ChunkFront),
			ChunkFrontRight(_ChunkCentreFront),
			NTMCG_Base(_DimXZ, _DimY, ChunkX, ChunkZ)
		{
			regenerateEnvelope();
		}

		void Generate();
		void Update();

		MidPointBase* getVertexVectorElement(int i);
		const int getVertexVectorSize();

		int getIndexVectorElement(int i);
		const int getIndexVectorSize();
	};

	/**!
	Does the same thing as usual generator, but does't separate vertices
	*/
	class NodeToMCGeneratorNaive : public NTMCG_Base
	{
		//A chunks. From them a mesh will be generated
		std::shared_ptr<GEM::NodeChunk> ChunkCentre, ChunkRight, ChunkFront, ChunkFrontRight;

		//structures should be public
	public:
		/**!
		Nodes that comes from MapGenerator contains only things, specific to that node. But we also need some way to access mid-points, associated
		with given node. So we just do an envelope that includes that midpoints.
		According to a paper about MarchingCubes, Midpoints that are represented here lays on a 0,3,8
		MidPoints should be set to a value, that is weighted average of nodes on their edge, assuming that node [x][y][z] have cordinates(x;y;z)
		*/
		struct NodeEnvelope {
			MidPointBase Right, Back, Top;
			bool isChanged = false;
		};
	private:

		std::vector<std::vector<std::vector<NodeEnvelope>>> NodeEnvelopeVec;
		//This fector is flat. It must be accessed in the order values have been added in it
		std::vector<unsigned char> m_cubeValuesCash;

		//Holds EdgePoints in order and with duplications
		std::vector<MidPointBase> VertexVector;
		//Holds IDs from VertexVector, so that they could be arranged in triangles
		std::vector<int> IndexVector;

		/**!
		Acesses node from multiple NodeMap as they are one big NodeMap.
		\param[in] NodeX Position of a node in a big NodeMap
		\param[in] NodeY Position of a node in a big NodeMap
		\param[in] NodeZ Position of a node in a big NodeMap

		\note This method have really narrow contract. It assumes, that user insert correct data and does not perform any checks.
		It also actively uses data of instance that it's called from, so...
		NodeX MUST be between 0 and DimXZ+1;
		NodeZ MUST be between 0 and DimXZ+1;
		NodeY MUST be between 0 and DimY;
		*/
		inline Node& GetNode(int NodeX, int NodeY, int NodeZ)
		{
			if ((NodeX >= DimXZ))
			{
				if ((NodeZ >= DimXZ))
				{
					return ChunkFrontRight->NodeMap[NodeX - DimXZ][NodeY][NodeZ - DimXZ];
				}
				else
				{
					return ChunkRight->NodeMap[NodeX - DimXZ][NodeY][NodeZ];
				}
			}
			else if (NodeZ >= DimXZ)
			{
				return ChunkFront->NodeMap[NodeX][NodeY][NodeZ - DimXZ];
			}
			else
			{
				return ChunkCentre->NodeMap[NodeX][NodeY][NodeZ];
			}
		}

		/**!
		Access Nodes as they are in a Cubie.
		\param[in] CubeX X-cordinate of a cubie
		\param[in] CubeY Y-cordinate of a cubie
		\param[in] CubeZ Z-cordinate of a cubie
		\param[in] NodeID Id of a node in a cubie, according to a MarchingCubes paper
		*/
		inline Node& GetNodeAsCubie(int CubeX, int CubeY, int CubeZ, int NodeID)
		{
			switch (NodeID)
			{
			case 0: return GetNode(CubeX, CubeY, CubeZ);
			case 1: return GetNode(CubeX + 1, CubeY, CubeZ);
			case 2: return GetNode(CubeX + 1, CubeY, CubeZ + 1);
			case 3: return GetNode(CubeX, CubeY, CubeZ + 1);

			case 4: return GetNode(CubeX, CubeY + 1, CubeZ);
			case 5: return GetNode(CubeX + 1, CubeY + 1, CubeZ);
			case 6: return GetNode(CubeX + 1, CubeY + 1, CubeZ + 1);
			case 7: return GetNode(CubeX, CubeY + 1, CubeZ + 1);
			}
		}

		/**!
		Finds a value of a cube, according to a paper
		*/
		inline int GetCubieValue(int CubeX, int CubeY, int CubeZ)
		{
			int cubeindex = 0;
			if (GetNodeAsCubie(CubeX, CubeY, CubeZ, 0).Value > 0) cubeindex |= 1;
			if (GetNodeAsCubie(CubeX, CubeY, CubeZ, 1).Value > 0) cubeindex |= 2;
			if (GetNodeAsCubie(CubeX, CubeY, CubeZ, 2).Value > 0) cubeindex |= 4;
			if (GetNodeAsCubie(CubeX, CubeY, CubeZ, 3).Value > 0) cubeindex |= 8;
			if (GetNodeAsCubie(CubeX, CubeY, CubeZ, 4).Value > 0) cubeindex |= 16;
			if (GetNodeAsCubie(CubeX, CubeY, CubeZ, 5).Value > 0) cubeindex |= 32;
			if (GetNodeAsCubie(CubeX, CubeY, CubeZ, 6).Value > 0) cubeindex |= 64;
			if (GetNodeAsCubie(CubeX, CubeY, CubeZ, 7).Value > 0) cubeindex |= 128;
			return cubeindex;
		}

		/**!
		Calculates MidPoint on a specified Edge
		\param[in] CubeX Cordinate of a cube
		\param[in] CubeY Cordinate of a cube
		\param[in] CubeZ Cordinate of a cube
		\param[in] EdgeID ID of edge, which MidPoint must be calculated
		*/
		MidPointBase& CalcMidPoint(int CubeX, int CubeY, int CubeZ, int EdgeID);

		/**!
		Processes once Cube. Calculates its value, sets midpoints, creates triangles
		This method should be called only once for every call to Generate()
		*/
		void CreateCube(int CubeX, int CubeY, int CubeZ);

		/**!
		Updates once Cube. Calculates its value, sets midpoints, creates triangles
		This method should be called after CreateCube, untill Generate get's called?
		*/
		void UpdateCube(int CubeX, int CubeY, int CubeZ);

		/**!
		Creates new envelope.
		Assuming that it's empty
		*/
		inline void regenerateEnvelope()
		{
			NodeEnvelopeVec.resize(DimXZ + 1);
			for (int x = 0; x < DimXZ + 1; x++)
			{
				NodeEnvelopeVec[x].resize(DimY);
				for (int y = 0; y < DimY; y++)
				{
					NodeEnvelopeVec[x][y].resize(DimXZ + 1);
				}
			}
		}

	public:
		NodeToMCGeneratorNaive(std::shared_ptr<GEM::NodeChunk> _ChunkCentre, std::shared_ptr<GEM::NodeChunk> _ChunkRight, std::shared_ptr<GEM::NodeChunk> _ChunkFront, std::shared_ptr<GEM::NodeChunk> _ChunkCentreFront, int _DimXZ, int _DimY, int ChunkX, int ChunkZ)
			:
			ChunkCentre(_ChunkCentre),
			ChunkRight(_ChunkRight),
			ChunkFront(_ChunkFront),
			ChunkFrontRight(_ChunkCentreFront),
			NTMCG_Base(_DimXZ, _DimY, ChunkX, ChunkZ)
		{
			regenerateEnvelope();
		}

		void Generate();
		void Update();

		/**!
		Mark node as changed so that it will be redrawn at next Update
		*/
		void ChangeNode(int x, int y, int z, unsigned char newval);

		MidPointBase* getVertexVectorElement(int i);
		const int getVertexVectorSize();

		int getIndexVectorElement(int i);
		const int getIndexVectorSize();
	};

}