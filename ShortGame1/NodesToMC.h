#pragma once
#include "NodeChunk.h"
#include <cassert>

namespace GEM
{
	/**
	Generates MarchingCubes Mesh out of NodeChunk

	Generator actually pulls chunks from chunkLoader by itself. This is done so that later it could store
	mesh-related data and do a quick rebuilding of a mesh, or just return a pre-built version of it
	*/
	class NodesToMCGenerator {
	public:

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
			Node* N = nullptr;
			MidPoint Right, Back, Top;

			NodeEnvelope() {}
			NodeEnvelope(Node* _N) : N(_N) {}


			/**!
			Returns a node for a given Cube and NodeID
			\param[in] CubeX X-cordinate of a cubie
			\param[in] CubeY Y-cordinate of a cubie
			\param[in] CubeZ Z-cordinate of a cubie
			\param[in] NodeID ID of a node, accordig to a MarchngCubes paper
			\param[in] NodeMap A head pointer of 3-dimensional array of standartized size. Yes I tryed with std::array, everything just became worse
			*/
			inline Node& GetNode(int cubeX, int CubeY, int CubeZ, int NodeID, Node*** NodeMap)
			{
				switch (NodeID)
				{
				case 0: return NodeMap[cubeX][CubeY][CubeZ];
				case 1: return NodeMap[cubeX + 1][CubeY][CubeZ];
				case 2: return NodeMap[cubeX + 1][CubeY][CubeZ + 1];
				case 3: return NodeMap[cubeX][CubeY][CubeZ + 1];

				case 4: return NodeMap[cubeX][CubeY + 1][CubeZ];
				case 5: return NodeMap[cubeX + 1][CubeY + 1][CubeZ];
				case 6: return NodeMap[cubeX + 1][CubeY + 1][CubeZ + 1];
				case 7: return NodeMap[cubeX][CubeY + 1][CubeZ + 1];
				}
			}


		};

		/**!
		A cubie. Contains pointers to a node envelopers
		This is done, becouse Cubie is actually just a way of representation of nodes, and multiple cubies can(and usually do) include same nodes.
		The only problem is processor cache. Yeah. Two layers of pointers in ~32000 operations. But I just don't see any better solutions. The tests will show,
		how bad its actually is
		*/
		struct Cubie {
			/**
			ID's from http://paulbourke.net/geometry/polygonise/
			And all the scheme, actually; And (0,0,0) node is left front bottom
			*/

			//Cubie nodes
			NodeEnvelope* N0;
			NodeEnvelope* N1;
			NodeEnvelope* N2;
			NodeEnvelope* N3;

			NodeEnvelope* N4;
			NodeEnvelope* N5;
			NodeEnvelope* N6;
			NodeEnvelope* N7;

			unsigned short cubeval = 0;

			/**!
			Calculates cubeval. All nodes must be set before this function get's called!
			*/
			void CalculateCubeval()
			{
			/*	assert((N0.N != nullptr) && (N1.N != nullptr) && (N2.N != nullptr) && (N3.N != nullptr) && (N4.N != nullptr) && (N5.N != nullptr) && (N6.N != nullptr) && (N7.N != nullptr));
				/*Some of nodes not beeing set!*/
			/*	cubeval = 0;
				if (N0.N->Value > 0) cubeval |= 1;
				if (N1.N->Value > 0) cubeval |= 2;
				if (N2.N->Value > 0) cubeval |= 4;
				if (N3.N->Value > 0) cubeval |= 8;
				if (N4.N->Value > 0) cubeval |= 16;
				if (N5.N->Value > 0) cubeval |= 32;
				if (N6.N->Value > 0) cubeval |= 64;
				if (N7.N->Value > 0) cubeval |= 128;*/
			}

		};
		NodesToMCGenerator(ChunkLoader<NodeChunk>* chunkLoader);

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
		void ProcessCube(std::vector<std::vector<std::vector<Cubie>>> &Cubies, int x, int y, int z);

		/**!
		Allows access to a specified MidPoint, by Cube cordiantes and Edge ID
		\param[in] Cubies A reference to a cubies array
		\param[in] CubeX X-cordinate of a cube
		\param[in] CubeY Y-cordinate of a cube
		\param[in] CubeZ X-cordinate of a cube
		\param[in] EdegeID ID of an edge. According to MarchingCube paper
		\returns Returns a reference to a requested midpoint
		*/
		MidPoint& getMidPoint(std::vector<std::vector<std::vector<Cubie>>> &cubies, int cubeX, int cubeY, int cubeZ, int edgeID);

		ChunkLoader<NodeChunk>* m_chunkLoader;

	};
}