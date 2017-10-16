#pragma once
#include "NodeChunk.h"

namespace GEM
{
	/**!
	Holds all needed data to generate one MarchingCube Nesh for one chunk.

	The idea is that GeneratorController will create Generator for every chunk that should be created, provide them with all needed data
	and then lets them generate meshes
	*/
	class NodesToMCGenerator
	{
	private:
		//A chunks. From them a mesh will be generated
		std::shared_ptr<GEM::NodeChunk> ChunkCentre, ChunkRight, ChunkFront, ChunkFrontRight;

		//DimXZ - Dimensions X and Z of a NodeChunk. Just in case if they would change. DimY is the same, but for Y		
		int DimXZ, DimY;

		/**!
		We actually wants to find a points on edges of cubes, that would be a part of iso-surface.
		MidPoint is a representation of those points. It's just a float triplet of (x,y,z)
		MidPoint counts as Unset if X or Y or Z is -1; That's their default state
		*/
		struct MidPoint {
			float x, y, z;
			int VertexVectorPosition = -1;//Position in a VVP this point occupy(Or -1 if it's not in a vector). Used to avoid multiple enterence
			MidPoint(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
			MidPoint() : x(-1), y(-1), z(-1) {}
		};

		/**!
		Nodes that comes from MapGenerator contains only things, specific to that node. But we also need some way to access mid-points, associated
		with given node. So we just do an envelope that includes that midpoints.
		According to a paper about MarchingCubes, Midpoints that are represented here lays on a 0,3,8
		MidPoints should be set to a value, that is weighted average of nodes on their edge, assuming that node [x][y][z] have cordinates(x;y;z)
		*/
		struct NodeEnvelope {
			MidPoint Right, Back, Top;
		};


		std::vector<std::vector<std::vector<NodeEnvelope>>> NodeEnvelopeVec;

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

		/*
		inline MidPoint& GetMidPoint(int CubeX, int CubeY, int CubeZ, int EdgeID)
		{
			/**!
			So we're trying to do another Map, that contains only EdgePoint, but positionally similar to NodeMap, so that we could use the same trick
			and access Edges like they are in Cubie
			*/
			/*switch (EdgeID)
			{
			case 0: return NodeEnvelopeVec[CubeX][CubeY][CubeZ].Right;
			case 1: return NodeEnvelopeVec[CubeX + 1][CubeY][CubeZ].Back;
			case 2: return NodeEnvelopeVec[CubeX][CubeY][CubeZ + 1].Right;
			case 3: return NodeEnvelopeVec[CubeX][CubeY][CubeZ].Back;

			case 4: return NodeEnvelopeVec[CubeX][CubeY + 1][CubeZ].Right;
			case 5: return NodeEnvelopeVec[CubeX + 1][CubeY + 1][CubeZ].Back;
			case 6: return NodeEnvelopeVec[CubeX][CubeY + 1][CubeZ + 1].Right;
			case 7: return NodeEnvelopeVec[CubeX][CubeY + 1][CubeZ].Back;

			case 8: return NodeEnvelopeVec[CubeX][CubeY][CubeZ].Top;
			case 9: return NodeEnvelopeVec[CubeX + 1][CubeY][CubeZ].Top;
			case 10: return NodeEnvelopeVec[CubeX + 1][CubeY][CubeZ + 1].Top;
			case 11: return NodeEnvelopeVec[CubeX][CubeY][CubeZ + 1].Top;
			}
		}*/

		/**!
		Calculates a CubeValue for a cubie
		\param[in] CubeX X-cordinate of a cube
		\param[in] CubeY Y-cordinate of a cube
		\param[in] CubeZ Z-cordinate of a cube
		\returns CubeValue of a Cubie
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
		*/
		void ProcessCube(int CubeX, int CubeY, int CubeZ);

	public:
		NodesToMCGenerator(std::shared_ptr<GEM::NodeChunk> _ChunkCentre, std::shared_ptr<GEM::NodeChunk> _ChunkRight, std::shared_ptr<GEM::NodeChunk> _ChunkFront, std::shared_ptr<GEM::NodeChunk> _ChunkCentreFront, int _DimXZ, int _DimY)
			:
			ChunkCentre(_ChunkCentre),
			ChunkRight(_ChunkRight),
			ChunkFront(_ChunkFront),
			ChunkFrontRight(_ChunkCentreFront),
			DimXZ(_DimXZ),
			DimY(_DimY)
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

		void Generate();

		std::vector<MidPoint*> &getVertexVector();
		std::vector<int> &getIndexVector();
	};

}