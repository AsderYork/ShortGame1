#include "stdafx.h"
#include "VisualController.h"

namespace GEM
{

	void VisualController::StartBackgroundInit()
	{
	}

	void VisualController::WaitForInit()
	{
		m_sky.StartVisualization();
	}

	void VisualController::Frame()
	{
		m_objCubical.Frame();
		m_sky.frame();
	}

	VisualController::~VisualController()
	{
		;
	}

}
