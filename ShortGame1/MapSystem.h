#pragma once
#include "CEGUI_Service.h"
#include "Ogre_Service.h"
#include <OGRE\OgreVector2.h>
#include "NodesToMC.h"

namespace GEM
{
	/**
	The second generation of a map system.
	This class controlls, the appearance of a game map.
	Its a GEM_Service. It depends on Ogre_Service for map rendering, and on CEGUI_Service for debug overlays
	*/
	class MapSystem {
	public:
		/**
		MapSystem Requires CEGUI and OGRE to show a map and related debug overlays
		*/
		MapSystem(CEGUI_Service* CeguiService, Ogre_Service* OGREService);
		/**
		Sets the position of a player. Using this method, MapSystem can know, which parts of a map should be drawn
		*/
		void SetPlayerPos(float x, float y);

		/**
		Redraws if needed all the map. If Map havn't been change from last call, and the Player is on the same'ie position, then nothing
		will be actualy redrawn, otherwise all the changes will be commited to and redrawn, while unchanged parts will stay.
		*/
		void DrawMap();

		/**
		Vision size controlls how big the size of a part of a map, that will be visible for a player. The bigger the value, the more
		expensive it becomes, and the more world is visible at a time.
		*/
		void SetVisionRadius(int Radius);
		int GetVisionRadius();

	private:
		/**
		Amount of chunks that should be loaded around player.
		*/
		int m_visionRadus = 2;
		CEGUI_Service* m_ceguiService;
		Ogre_Service* m_ogreService;
		Ogre::Vector2 m_playerPos;

			
	};
}