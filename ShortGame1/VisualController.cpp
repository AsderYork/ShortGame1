#include "stdafx.h"
#include "VisualController.h"

namespace GEM
{

	void VisualController::Initialize()
	{
		m_sky.StartVisualization();
	}

	void VisualController::Frame()
	{
		m_sky.frame();
	}

}
