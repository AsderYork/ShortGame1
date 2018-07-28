#pragma once
#include "Ogre_Service.h"
#include "GameSimService.h"

#include <map>

namespace GEM
{
	/**!
	Visualizes all objects as boring white cubes
	This is a debug class
	*/
	class ObjectVis_Cubical
	{
	private:
		GameSimController * m_gsController;
		Ogre_Service* m_ogreController;


		struct ObjectRAII
		{
		public:
			Ogre::Item* item;
			Ogre::SceneNode* node;
			Ogre::SceneNode* PlayerNode;

			ObjectRAII(const ObjectRAII&) = delete;
			ObjectRAII(ObjectRAII&& other);

			ObjectRAII();
			~ObjectRAII();
		};

		std::map<GameSim::ENTITY_ID_TYPE, ObjectRAII> m_visibleEntities;
	public:
		ObjectVis_Cubical(GameSimController* gs);
		~ObjectVis_Cubical();

	
		void Frame();

	};
}