#pragma once
#include <vector>
#include "ChunkLoader.h"
#include <OGRE\OgreVector3.h>
#include <OGRE\OgreVector2.h>

namespace GEM
{
	class MarchingCubiesMeshCalc
	{		
		/**
		Every node now can be in a 3 different flavors. UpDown, LeftRight, FrontBack. And the idea of arranging them in a three separte sub-meshes seems good.
		But it'll breake normals. So we actually will just mark all nodes with their flavor. And before VertexList, they will be added in a RefferenceList.

		When we adding vertex to a RefferenceList, we check if it was allready marked with it's flavor. If it is, then it must be allready in a RefferenceList, so
		we just adding it's ID to IndexList. if it's not marked for current flavor, then it must be new vertex. Add it to a RefferenceList, and sed Index;

		ReferencList can contain multiple occurences of a Node. And it will be translated to actual VertexList without changes to IndexList.
		ReferenceList will contain refference to a Vertex and flavor, in which it is added. In a pair. So when translating RefferenceList to a VertexList,
		one should take all the parameters of index as-is, and then set textCord according to a flavor, in which vertex were used.

		IndexList still will be unified.

		
		*/
		public:
			struct vertexData {
				Ogre::Vector3 pos;
				Ogre::Vector3 normal;
				Ogre::Vector2 texCord;
			};

			struct intvector3 {
				int x, y, z;
				intvector3(int _x, int _y, int _z) : x(_x), y(_y), z(_z) {}
				intvector3() : x(0), y(0), z(0) {}
			};

			struct EdgePoint {
				Ogre::Vector3 pos;
				Ogre::Vector3 normal;
				//flavors
				int UpDown = -1;
				int LeftRight = -1;
				int FrontBack = -1;

				void Drop() { UpDown = -1; LeftRight = -1;  FrontBack = -1; }
			};

			struct Node {
				int posX=0, posY=0, posZ=0;
				int Value;

				EdgePoint right;
				EdgePoint top;
				EdgePoint front;

				Node() : Value(0) {};
				Node(unsigned short _value) : Value(_value) {};
				void setPose(int _x, int _y, int _z) {
					posX = _x; posY = _y; posZ = _z;
				}

			};

			struct Cubes {
				Node n0, n1, n2, n3, n4, n5, n6, n7;
				unsigned short CubeValue;
			};

			struct MeshVertex {
				Ogre::Vector3 pos;
				Ogre::Vector3 normal;
				Ogre::Vector2 TextureCord;

				MeshVertex(EdgePoint &EP) : pos(EP.pos), normal(EP.normal) {}
				MeshVertex(EdgePoint *EP, float u, float v) : pos(EP->pos), normal(EP->normal), TextureCord(u, v){}
				MeshVertex(Ogre::Vector3 _pos, Ogre::Vector3 _norm, float u, float v) : pos(_pos), normal(_norm), TextureCord(u, v) {}
				MeshVertex() : pos(Ogre::Vector3::ZERO), normal(Ogre::Vector3::ZERO), TextureCord(Ogre::Vector2::ZERO){}

			};


			MarchingCubiesMeshCalc();


			void CalculateMesh(float scale, float posx, float posy);

			int getMapSizeX();
			int getMapHeight();
			int getValueOfNode(int x, int y, int z);
			void setValueOfNode(int x, int y, int z, int Value);
			void FillNodeList(Chunk* chunk);

			std::vector<MeshVertex>& getVertexes();
			std::vector<int>& getIndexes();
					
		private:
		
			void processOneCube(int x, int y, int z);
			//void setVertexPos(int Cubex, int Cubey, int Cubez, int VertexNum);
			EdgePoint* setEdgePoint(int Cubex, int Cubey, int Cubez, int EdgeNum);
		

			Node& fromXYZ(int x, int y, int z);

			Ogre::Vector3 FindEdgePointPosition(Node node1, Node node2);

			int PushEdgePointInRefferenceList(EdgePoint* EdgePoint, int Flavor);
		

			float m_scale = 1;
			int m_mapSize = 8;

			std::vector<Node> m_nodeList;
			std::vector<std::pair<EdgePoint*, int>> m_refferenceList;
			std::vector<MeshVertex> m_meshVerticies;
			std::vector<int>  m_indeces;
			/**
			In paper listened bellow was a picture if a cube, where vertices and edges where asigned with number. We use same numbers
			*/
			Node& findCubeNodeFromCords(int x, int y, int z, int NodeID);

			void RebuildToACube();


	};
}