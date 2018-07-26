#pragma once

#include "SkyVisualization.h"

namespace GEM
{
	/**!
	Controlls everything, that is responsible for GameState Visualization
	*/
	class VisualController
	{
		SkyVisualization m_sky;
	public:

		void Initialize();
		void Frame();

	};
}