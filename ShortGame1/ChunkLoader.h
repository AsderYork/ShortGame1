#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>

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
		inline int SqueredistanceTo(const intpos2& rhs) const { return (x - rhs.x)*(x - rhs.x) + (y - rhs.y)*(y - rhs.y); };

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

		template<class Archive>
		void serialize(Archive & archive) { 
			archive(Nodes, sizeof(MapNode)*CHUNK_SIZE*CHUNK_SIZE*CHUNK_HEIGHT);
			archive(ChunkPos);
			archive(id, sizeof(long long));
		}

		~Chunk();
	};

	/**
	Loads chunks.
	It's a singleton class.
	Creation is not thread-safe. But the Loader itself do uses threads.

	Chunk loader do loads chunks. It's even can save them! But it knows nothing about which chunk should be loaded.
	When you load up a chunk, it will remain in memory. Even if you'll try to recive it again, you'll only get another shared_ptr to a same block
	of memory. But if noone currently holds a pointer, the chunk will fall into the oblivion! preliminarily saved.

	*/

	class ChunkLoader {
	public:
		ChunkLoader();
		static ChunkLoader* GetOrCreateChunkLoader();

		~ChunkLoader();
		std::shared_ptr<Chunk> getChunk(intpos2 pos);
		void SaveMagistral();


	private:
		inline int IDfromXY(intpos2 XY) {
			return XY.x*MAP_SIZE + XY.y;
		};

		void InitializeChunkLoader();
		Chunk* loadChunk(intpos2 pos);
		void SaveChunk(Chunk* chunk);

		/**If there is no chank, it's possible to create it*/
		Chunk* CreateChunk(intpos2 pos);


		/**
		When map is loaded or created, it contains a list of all chunks, that it have. Here it is
		*/
		std::vector<intpos2> m_magistral;		

		static ChunkLoader* m_singleton;
		std::string m_mapFolder;


		/**
		All chunks, that is loaded right now, contains here
		*/
		std::vector<std::pair<std::weak_ptr<Chunk>, intpos2>> m_loadedChunks;

		friend struct Chunk;
	};

}