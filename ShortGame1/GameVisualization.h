#pragma once
#include "Ogre_Service.h"
#include "GameSimService.h"

#include <map>
//#include <OGRE\OgreItem.h>

namespace GEM
{
	/**!
	This is a temporary class. It's purpose is
	to connect game simulation with Ogre, so that player's entity would be visible on a screen
	*/
	class GameVisualization : public Service
	{
	private:
		GameSimController * m_gsController;
		Ogre_Service* m_ogreController;

		struct ObjectRAII
		{
		private:
			Ogre_Service* ogre;
		public:
			Ogre::Item* item;
			Ogre::SceneNode* node;

			ObjectRAII(const ObjectRAII&) = delete;
			ObjectRAII(ObjectRAII&& other);

			ObjectRAII(Ogre_Service* Ogre);
			~ObjectRAII();
		};

		std::map<GameSim::ENTITY_ID_TYPE, ObjectRAII> m_visibleEntities;
	public:
		GameVisualization(GameSimController* gs, Ogre_Service* ogre);

	
		virtual ActionResult initialize() override;

		virtual void shutdown() override;

		virtual ActionResult preFrame(float timeDelta) override;

		virtual ActionResult frame(float timeDelta) override;

		virtual ActionResult postFrame(float timeDelta) override;

	};
}