#pragma once
#include "NodeChunk.h"
#include <memory>
#include <vector>
#include <list>
#include <OGRE\OgreVector3.h>

namespace GEM
{
	/**!
	Does the same thing as usual generator, but does't separate vertices
	*/
	class NodeToMCGeneratorNaive
	{
		static const int triTable[256][16];
		int DimXZ, DimY;


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

		struct MidPointBase
		{
			//float x = 0, y = 0, z = 0;
			Ogre::Vector3 normal, pos, textcord1;
			//float nx = 0, ny = 1, nz = 0;

			//Texture cordinate are actually 3d. But with a help of a flavors ony 2 of cordinate will end in actual mesh
			//float uvx = 0, uvy = 0, uvz = 0;
			//This also means that MCToMesh should be able to work differetiate flavors correctly, choosing right u, v pair for a given flavor.
		};

		struct NodeEnvelope {
			MidPointBase Right, Back, Top;
			bool isChanged = false;
		};

		enum class Flavor { FLAVOR_UPDOWN, FLAVOR_LEFTRIGHT, FLAVOR_FRONTBACK };
	private:

		//This thing is moved out of CreateCube to prevent its rebuild every time, and keep it to the instance of a class opposing to static
		std::vector<std::pair<MidPointBase*, Flavor>> m_vertices;
		std::vector<std::vector<std::vector<NodeEnvelope>>> NodeEnvelopeVec;

		//Holds EdgePoints in order and with duplications
		std::vector<std::pair<MidPointBase*,Flavor>> VertexVector;
		//IndexVector is no longer used, becouse they all just sequential
		//std::vector<int> IndexVector;

		//Linear representation. Contains chunks, that heve been changed from last Update/Generate.
		std::vector<int> ChangedCubies;


		struct CubeData {
			
			/**
			If this cube contains any additional geometry, it's vertex buffer will be allocated in a list, marked with
			this iterator
			*/
			std::list<std::vector<std::pair<MidPointBase*, Flavor>>>::iterator PosInActiveList;
			//True if Iterator is a valid one. False otherwise
			bool isSet = false;
			unsigned char lastCubeVal = 0;

			CubeData(std::list<std::vector<std::pair<MidPointBase*, Flavor>>>::iterator&& it) :
				PosInActiveList(it)
			{}
			CubeData() {}

		};

		/*Contains data about cubes, so that they can be updated. This vector is inherently sorted after Generate
		It's get filled complitely after Generate. At update elements must be only altered, no eddition or removal
		*/
		std::vector<CubeData> m_cubeData;
		/*Contains IDs of cubes, that do have nodes. The vector is sorted after Generate. It must remain sorted after any changes in Update.
		*/
		std::list<std::vector<std::pair<MidPointBase*, Flavor>>> m_actuallyUsedCubesVertices;

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
#pragma warning( push )
#pragma warning( disable : 4715)//No-return case is actually an assert
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
			default: {assert(false);/*Cuibie have only 8 nodes!*/}
			}
		}
#pragma warning( pop )

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

		//Transforms x,y,z cordinates of a cube in to a linear cordinate
		inline int XYZToLinearCube(int x, int y, int z)
		{
			return DimXZ*(DimY - 1)*x + DimXZ*y + z;
		}
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

		void RemoveNormals(int CubeX, int CubeY, int CubeZ, unsigned char cubeval);
		

	public:
		NodeToMCGeneratorNaive(std::shared_ptr<GEM::NodeChunk> _ChunkCentre, std::shared_ptr<GEM::NodeChunk> _ChunkRight, std::shared_ptr<GEM::NodeChunk> _ChunkFront, std::shared_ptr<GEM::NodeChunk> _ChunkCentreFront, int _DimXZ, int _DimY, int ChunkX, int ChunkZ)
			:
			ChunkCentre(_ChunkCentre),
			ChunkRight(_ChunkRight),
			ChunkFront(_ChunkFront),
			ChunkFrontRight(_ChunkCentreFront),
			DimXZ(_DimXZ),
			DimY(_DimY)
		{
			regenerateEnvelope();
		}

		void Generate();
		void Update();

		/**!
		Mark node as changed so that it will be redrawn at next Update
		*/
		void ChangeNode(int x, int y, int z);

		/**
		Returns a pair of MidPointBase pointer and a flavor, in which this point is used in that place.
		A Normals is not normalized in this 
		*/
		const std::pair<MidPointBase*, Flavor> getVertexVectorElement(int i) const;
		const int getVertexVectorSize() const;

		int getIndexVectorElement(int i) const;
		const int getIndexVectorSize() const;
	};
}