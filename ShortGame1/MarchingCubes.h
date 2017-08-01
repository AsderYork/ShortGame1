#pragma once
#include <vector>

namespace GEM
{
	/**
	Marching cubes. Fun thing, actually. Here's the link to a http://paulbourke.net/geometry/polygonise/ source.
	*/
	class MarchingCubesCalculator
	{
	public:
		using VoxelValue = unsigned char;


		struct Vector3 {
			float x;
			float y;
			float z;
			Vector3() : x(0), y(0), z(0) {}
			Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
		};


		struct EdgePoint
		{
			Vector3 pos;//Position of intermediate point
			int Index;//Index in index array, if this point is used. Otherwise index must be -1;

			EdgePoint() : Index(-1) {  }
			EdgePoint(int _index, Vector3 _pos) : Index(_index), pos(_pos){  }
		};

		struct Node
		{
			//EdgePoints should be left unused if node is at top, front or left edge of a mesh
			EdgePoint top;
			EdgePoint front;
			EdgePoint left;

			int posX;
			int posY;
			int posZ;

			unsigned short value;
			Node() : value(0) {};
			Node(unsigned short _value) : value(_value) {};
			void setPose(int _x, int _y, int _z) {
				posX = _x; posY = _y; posZ = _z;
			}
		};

		struct Cube
		{
			Node n0;
			Node n1;
			Node n2;
			Node n3;
			Node n4;
			Node n5;
			Node n6;
			Node n7;

		unsigned short CubeValue;
			Cube(unsigned short _CubeValue) : CubeValue(_CubeValue) {};
			Cube() : CubeValue(0) {};
		};


	

		MarchingCubesCalculator();

		const std::vector<std::vector<std::vector<Node>>> getMap();

		/**
		Creates a mesh out of map.
		\param[in] posx Position from where should mesh be started
		\param[in] posy Position from where should mesh be started
		\param[in] scale How big should mesh appear in a world
		*/
		void calculateMesh(float posx, float posy, float scale);

		std::vector<Vector3> &getVertexes();
		std::vector<int> &getIndexes();


		void setValueOfNode(int x, int y, int z, VoxelValue value);
		int getValueOfNode(int x, int y, int z);

	private:
		template<class T>
		using Vector3d = std::vector<std::vector<std::vector<T>>>;

		Vector3 FindEdgePointPosition(Node node1, Node node2, float Scale);
		
		
		/**
		In the paper the where id's of edges. This method allowes us to access edges by this id
		*/
		EdgePoint& GetEdgepointByNumber(Cube &Cube, int id);

		inline void SetPositionAndIndexOfEdge(Cube &Cube, int EdegeID, int tmpIndex, float Scale) {
			GetEdgepointByNumber(Cube, 0).pos = FindEdgePointPosition(Cube.n0, Cube.n1, Scale);
			if (GetEdgepointByNumber(Cube, 0).Index == -1) { GetEdgepointByNumber(Cube, 0).Index = tmpIndex++; }
		}

		/**
		\return Returns Index of Edge
		This method must be called only for Edges, that really appear in a mesh. Otherwise unused Edgepoints will be indexed
		*/
		int SetEdgePoint(Cube &Cube, int EdegeID, float Scale);

		std::vector<std::vector<std::vector<Node>>> m_map;

		std::vector<Vector3> m_VertexVector;
		std::vector<int> m_IndexVector;
	};

}