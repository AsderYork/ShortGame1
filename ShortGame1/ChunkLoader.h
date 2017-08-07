#pragma once

#include <string>
#include <vector>
#include <map>

#define CHUNK_SIZE 32
#define CHUNK_HEIGHT 128

/**
The map is not endless.
*/
#define MAP_SIZE 2048




namespace GEM
{
	struct intpos2 {
		int x = 0;
		int y = 0;
		intpos2() {}
		intpos2(int _x, int _y) : x(_x), y(_y) {}
		inline bool operator==(const intpos2& rhs) const { return ((x == rhs.x) && (y == rhs.y)) ? true : false; }
		inline bool operator!=(const intpos2& rhs) const { return ((x == rhs.x) && (y == rhs.y)) ? false : true; }
		//std::map uses this operator
		inline bool operator<(const intpos2& rhs) const { return ((x*MAP_SIZE + y) < (rhs.x*MAP_SIZE + rhs.y)) ? true : false; } ;

		template<class Archive>
		void serialize(Archive & archive){archive(x, y);}
	};

	struct MapNode {
		unsigned char Value=0;
		template<class Archive>
		void serialize(Archive & archive) { archive(Value); }
	};
	struct Chunk {
		MapNode Nodes[CHUNK_SIZE][CHUNK_HEIGHT][CHUNK_SIZE];
		/*
		Chinks have a position; The position of a chunk, is the x,z cordinates of back, left block.
		*/
		intpos2 ChunkPos;
		long long id;
		~Chunk();
	};

	/**
	Loads chunks.
	It's a singleton class.
	Creation is not thread-safe. But the Loader itself do uses threads.

	Chunk loader do loads chunks. It's even can save them! But it knows nothing about which chunk should be loaded, and when they must be unloaded
	calling delete on a chunk will couse it to do all the things to save manage itself properly.

	*/

	class ChunkLoader {
	public:
		ChunkLoader();
		~ChunkLoader();
		bool InitializeChunkLoader();
		Chunk* getChunk(intpos2 pos);
		void SaveMagistral();


	private:
		inline int IDfromXY(intpos2 XY) {
			return XY.x*MAP_SIZE + XY.y;
		};

		Chunk* loadChunk(intpos2 pos);
		void SaveChunk(Chunk* chunk);

		/**If there is no chank, it's possible to create it*/
		Chunk* CreateChunk(intpos2);

		/**
		When map is loaded or created, it contains a list of all chunks, that it have. Here it is
		*/
		std::vector<intpos2> m_magistral;		

		static ChunkLoader* m_singleton;
		std::string m_mapFolder;

		/**
		All chunks, that is loaded right now, contains here
		*/

		std::map<intpos2, Chunk*> m_loadedChunks;

		friend struct Chunk;
	};

}