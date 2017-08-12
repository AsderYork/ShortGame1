#include "stdafx.h"
#include "MapSystem.h"

namespace GEM
{
	MapSystem::MapSystem(CEGUI_Service * CeguiService, Ogre_Service * OGREService) :
		m_ceguiService(CeguiService),
		m_ogreService(OGREService)
	{
	}
	void MapSystem::SetPlayerPos(float x, float y)
	{
		m_playerPos = Ogre::Vector2(x, y);
	}

	void MapSystem::DrawMap()
	{
		//Remove chunks, that is now should not be visible.
		//Add chunks, that now isi visible.

		//Create a list of all chunks, that needs to be updated
			//All the chunks, that just been added, are automaticaly here.
			//Chunks, that was changed also go there.

		//Update chunks in a list
	}
	void GEM::MapSystem::SetVisionRadius(int Radius)
	{
		assert(Radius > 1);//Radius should never be less then 1

		m_visionRadus = Radius;
	}
	int MapSystem::GetVisionRadius()
	{
		return m_visionRadus;
	}
}