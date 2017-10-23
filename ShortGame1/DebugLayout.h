#pragma once
#include "CEGUI_Service.h"
#include <CEGUI\CEGUI.h>
#include <queue>
#include "Ogre_Service.h"
#include "MapService.h"
namespace GEM
{
	/**!
	A debug layout, that show FPS position and other useful info
	*/
	class DebugLayout : public CEGUI_Layout
	{
	public:
		/**
		To make FPS a little more useful, it need's to be avg'ed over multiple frames(otherwise it hopps too much);
		But if avg'ing too many frames, result will be less accurate
		*/
		DebugLayout(int SizeOfAvgGroup, Ogre_Service *OgreService, MapService* mapService=nullptr);
		virtual bool Initialize() override;
		virtual void turnOn() override;
		virtual void turnOff() override;
		virtual void PreFrame(float delta) override;

	private: 
		MapService* m_mapService;
		Ogre_Service *m_ogreService;
		CEGUI::Window* FPS_Window;
		int m_sizeOfAvgGroup;
		int m_avgFPS = 0;

	};
}