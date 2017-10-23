#pragma once
#include "EngineController.h"
#include "Ogre_Service.h"
#include "NodesToMC.h"
#include <list>

namespace GEM
{
	/**!
	A service. Create map so that it's allways around the player
	*/
	class MapService : public Service
	{
	private:
		Ogre_Service* m_ogreService;
		GEM::ChunkLoader<GEM::NodeChunk> m_loader;
		GEM::NodesToMCGeneratorController m_generator;

		int m_drawDistance = 1;

		/**!
		Chunks can be changed. So we track down, what chunks have been changed, and update their meshes every frame
		Every frame this list becomes clear. Even if some chunk were added in here, but wasn't redrawn that's just means that it wasn't drawn
		in the first place and so that there is no mesh needs updating.
		*/
		std::vector<std::pair<int, int>> m_changedChunks;

		/**!
		Tracks down loaded chunks.
		*/
		std::list<std::pair<int, int>> m_activeChunks;


		/**!
		Camera can move. And chunks must be loaded around it, and unloaded if they are too far.
		This metod does exactly that
		*/
		void ProcessCameraMovement();

	public:

		/**!
		Takes world cordinate and return a <int,int> pair of cordinates of a chunk, that these cordinates belongs to.
		*/
		std::pair<int, int> getChunk(float x, float y, float z);

		/**!
		Sets value of a node
		\param[in] NodeX X-cordinate of a node
		\param[in] NodeY Y-cordinate of a node. Must be between 0 and CHUNK_HEIGHT
		\param[in] Nodez Z-cordinate of a node
		\param[in] value A value to be set
		*/
		void SetIndividualNode(int NodeX, int NodeY, int NodeZ, unsigned char value);
		unsigned char getNodeVal(int NodeX, int NodeY, int NodeZ);

		MapService(Ogre_Service* OgreService) :
			m_ogreService(OgreService),
			m_loader("../Map/", ".map"),
			m_generator(&m_loader)
		{};
		// Унаследовано через Service
		virtual ActionResult initialize() override;
		virtual void shutdown() override;
		virtual ActionResult preFrame(double timeDelta) override;
		virtual ActionResult frame(double timeDelta) override;
		virtual ActionResult postFrame(double timeDelta) override;

	
	};
}