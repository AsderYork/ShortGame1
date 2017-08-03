#pragma once
#include "CEGUI_Service.h"
#include <CEGUI\CEGUI.h>
#include <queue>

namespace GEM {

	class FPS_Layout : public CEGUI_Layout
	{
	public:
		/**
		To make FPS a little more useful, it need's to be avg'ed over multiple frames(otherwise it hopps too much);
		But if avg'ing too many frames, result will be less accurate
		*/
		FPS_Layout(int SizeOfAvgGroup);
		virtual bool Initialize() override;
		virtual void turnOn() override;
		virtual void turnOff() override;
		virtual void PreFrame(float delta) override;
	private:
		CEGUI::Window* FPS_Window;
		int m_sizeOfAvgGroup;
		int m_avgFPS=0;
		
	};
}