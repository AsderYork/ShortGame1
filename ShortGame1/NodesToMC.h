#pragma once
#include "NodeChunk.h"
#include <cassert>
#include <array>

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
		MidPoint is a representation of thouse points. It's just float pair of (x,y)
		MidPoint counts us Unset if X or Y is -1; That's their default state
		*/
		struct MidPoint {
			float x, y;
			MidPoint(float _x, float _y) : x(_x), y(_y) {}
			MidPoint() : x(-1), y(-1) {}
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
			if ((NodeX > DimXZ))
			{
				if ((NodeZ > DimXZ))
				{
					return ChunkFrontRight->NodeMap[NodeX - DimXZ][NodeY][NodeZ - DimXZ];
				}
				else
				{
					return ChunkRight->NodeMap[NodeX - DimXZ][NodeY][NodeZ];
				}
			}
			else if (NodeZ > DimXZ)
			{
				return ChunkFront->NodeMap[NodeX][NodeY][NodeZ - DimXZ];
			}
			else
			{
				return ChunkCentre->NodeMap[NodeX][NodeY][NodeZ - DimXZ];
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

		inline MidPoint& GetMidPoint(int CubeX, int CubeY, int CubeZ, int EdgeID, std::vector<std::vector<std::vector<NodeEnvelope>>> &NodeEnvelopeVec)
		{
			/**!
			So we're trying to do another Map, that contains only EdgePoint, but positionally similar to NodeMap, so that we could use the same trick
			and access Edges like they are in Cubie
			*/
			switch (EdgeID)
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
		}




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
	
	};

	/**
	Generates MarchingCubes Mesh out of NodeChunk

	Generator actually pulls chunks from chunkLoader by itself. This is done so that later it could store
	mesh-related data and do a quick rebuilding of a mesh, or just return a pre-built version of it
	*/
	class NodesToMCGeneratorController {
	public:

		NodesToMCGeneratorController(ChunkLoader<NodeChunk>* chunkLoader);

		/**
		Generates chunk just from nodes
		*/
		void GenerateFromScratch(int x, int y);
	private:

		/**!
		Fills MidPoints of a cube, based on a values of it's nodes
		\param[in] Cubies A Cubies, that contains nodes from which nodes come from
		\param[in] x X-coordinate of a cubie to be processed
		\param[in] y Y-coordinate of a cubie to be processed
		\param[in] z Z-coordinate of a cubie to be processed

		Cubies don't have to have cubevalue, it will be assigned inside.
		*/
		//void ProcessCube(Node*** NodeMap, int x, int y, int z);

		ChunkLoader<NodeChunk>* m_chunkLoader;

	};


}