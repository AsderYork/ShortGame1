#pragma once
#include "NodeChunk.h"
#include <cassert>
#include <array>
#include "Ogre_Service.h"
#include "NodesToMCGenerator.h"
#include "NTMCG_Naive.h"
#include "MCToMesh.h"
#include <memory>
#include <atomic>
#include <mutex>

namespace GEM
{

	/**
	Controlls livetime of MarchingCubes Meshes.
	Intrenally it depends on ChunkLoader to pull neccessery information, on NodesToMCGenerator, to create Vertex and Index lists out of loaded maps, and on
	MCToMesh to Create actual meshes out of these lists
	*/
	class NodesToMCGeneratorController {
	public:

		NodesToMCGeneratorController(ChunkLoader<NodeChunk>* chunkLoader);

		/**!
		This method is deleted!
		*/
		//void GenerateFromScratch(int x, int y, Ogre_Service* ogreService);

		/**!
		Prepares the chunk, so that it could be accessed later.
		If specified chunk is allready preparing, prepared or drawn, this method will do nothing
		*/
		void PrepareChunk(int x, int z, Ogre_Service* ogreService);

		/**!
		Shows a chunk. If this chunk wasn't prepared before, this method will block untill it's done. If it was prepared, method will block
		untill chunk became shown. In other words, this method guarantees, that chunk will be shown.
		*/
		void ShowChunk(int x, int z, Ogre_Service* ogreService);

		/**!
		Unloads chunk, removing generator and MCtoMesh associated with it.
		If specified chunk wasn't loaded, nothing will happen.
		*/
		void UnloadChunk(int x, int z);

		/**!
		Update chunk that is allready created
		If this method is called for still loading chunk, method will block untill chunk is built
		*/
		void UpdateChunk(int x, int z, Ogre_Service* ogreService);

		/**
		Mark node as changed in corresponding generator, if there is one. Or just do nothing.
		This method dont perform actual node alteration! It's just tells generators, that cubes around node must be updated
		*/
		void ChangeNode(int ChunkX, int ChunkZ, int X, int Y, int Z);

		/**!
		Shut's down everything. This method might be called only once! It complitely shuts down NodesToMC
		*/
		void ShutDown();

		~NodesToMCGeneratorController();

	private:

		/**!
		Holds everything chunk-related.
		Chunk is PREPARING if Generator == nullptr
		Chunk is PREPARED if Generator != nullptr
		Chunk is MARKED if isActive == false; Marked chunk should be removed as soon as they became PREPARED
		Chunk is SHOWN if its Generator and Mesher are filled
		*/
		struct ChunkCore
		{
			std::unique_ptr<NTMCG_Base> Generator;
			std::unique_ptr<MCToMesh> Mesher;
			/**
			Before that the plan was to complitely move all generator creation to a worker threads. But the problem is that ChunkLoader is not
			thread-safe! So instead generators will be created during Prepare, and then worker threads will call Generate and mark chunk as Built
			*/
			std::atomic_bool isBuilt = false;
			/**
			Chunk can be deactivated using UnloadChunk method. But it is possible, that chunk will be under construction in this moment.
			If that is the case, then Generator will be null, and instead of removing chunk from a list, it will be marked as inactive. Then as soon
			as its done, ChunkLoader will fill Generator field and call UnloadChunk again.
			*/
			std::atomic_bool markedForDeletion = false;
			/*Only UnloadChunk can ask for delition but execute it can no only UnloadChunk, but also WorkerThread
			So the idea is that UnloadChunk grabs this mutex Mark this chunk then check if it's still not built
			If it's built, then it's guaranteed, that WorkerThread done all the work with this chunk so nothing will broke.
			If it's not set, then WorkerThread will wait, untill UnloadChunk ungrabs mutex. UnloadChunk gurantees that as soon as it grabbed
			a mutex, it can't do it again(Removal from m_listChunkUnit happens before mutex grabbing), and if it grabbed mutex and chunk were Prepared,
			it will unload it
			*/
			std::mutex DelitionRoutineMutex;

		};
		/**
		A list of all chunks, Shown and prepared.
		Chunk is SHOWN if it's MC and Mesh is created and it's actually visible for a palyer
		Chunk is PREPARED if it's MC is created and it can be updated.
		Chunk is UNLOADED if its not presented in this list
		*/
		std::list<ChunkCore> m_chunks;
		
		/**
		When Chunk need to be prepared, it goes to a working queue, were one of the worker threads will prepare it.
		While Chunk is in a workQueue it Can't be removed or updated. This List SHOULD NEVER BE TRAVESED! Everythig you add you must remember somewhere else
		*/
		std::list<std::list<ChunkCore>::iterator> m_workQueue;
		std::mutex m_workQueueMutex;
		std::atomic_bool m_ContinueThread = true;
		/**!
		Loads in a separate thread. Checks m_ntmcg_ToDo if Generator is needed to be created, then creates it and moves it to Done
		*/
		void ChunkLoaderThredFunc();
		std::thread m_workerThread;

		struct ChunkUnit
		{
			//Pos
			int x = 0, z = 0;
			std::list<ChunkCore>::iterator ChunkListIter;
			ChunkUnit(int _x, int _z, std::list<ChunkCore>::iterator CIL) : x(_x), z(_z), ChunkListIter(CIL) {}

		};

		/**!
		So we can't traverse m_chunks! Allright then! We could just write down every chunk we've been asked to prepare, then remove every chunks
		that we've been asked to remove. This vector holds a chunk position, so that it could be found, and an iterator in m_chunks, pointing to
		a said chunk. It is guaranteed, that all chunks, pointed that way will be valid.
		*/
		std::vector<ChunkUnit> m_ChunkUnits;



		////////////////////////////////////
		ChunkLoader<NodeChunk>* m_chunkLoader;	


	};


}