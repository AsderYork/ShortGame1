#pragma once
#include "CEGUI_Service.h"
#include <CEGUI\CEGUI.h>


namespace GEM {

	class MarchingCubeController : public CEGUI_Layout
	{
	public:

		// Унаследовано через CEGUI_Layout
		virtual bool Initialize() override;
		virtual void turnOn() override;
		virtual void turnOff() override;
		virtual void PreFrame(float delta) override;

	private:
		CEGUI::Window* m_Window;
	};
}