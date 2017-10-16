#pragma once
#include "EngineController.h"
#include "Ogre_Service.h"

namespace GEM
{
	/**!
	A service. Create map so that it's allways around the player
	*/
	class MapService : public Service
	{
	private:
		Ogre_Service* m_ogreService;

	public:
		MapService(Ogre_Service* OgreService) : m_ogreService(OgreService) {};
		// Унаследовано через Service
		virtual ActionResult initialize() override;
		virtual void shutdown() override;
		virtual ActionResult preFrame(double timeDelta) override;
		virtual ActionResult frame(double timeDelta) override;
		virtual ActionResult postFrame(double timeDelta) override;

	
	};
}