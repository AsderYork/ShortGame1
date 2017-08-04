#pragma once
#include "CEGUI_Service.h"
#include <CEGUI\CEGUI.h>


namespace GEM {

	class MarchingCubeController : public CEGUI_Layout
	{
	public:

		// ������������ ����� CEGUI_Layout
		virtual bool Initialize() override;
		virtual void turnOn() override;
		virtual void turnOff() override;
		virtual void PreFrame(float delta) override;

		void setChosedValue(int val);
		int getChosedValue();
		bool isActive();


	private:
		CEGUI::Window* m_window;
		CEGUI::Spinner* m_spinner;
		//CEGUI::Scrollbar* m_scrollbar;
	};
}