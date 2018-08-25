#pragma once

#include "SkyVisualization.h"
#include "GameSimService.h"
#include "LandscapeVisualizationController.h"
#include "FirstPersonGameCamera.h"

#include "ObjectVis_Cubical.h"

namespace GEM
{
	/**!
	Controlls everything, that is responsible for GameState Visualization
	This controller is responsible for drawing everything, that is a part of a GameSim.
	It must be avaliable, when actual game starts.

	If some of this controller's components would require specific notifications from GameSim about
	it's status, that should be done through GameSimController interface.

	It is possible, that later in the development there would be a need to perform resource acquisition
	in the background. It is adviced allow screens perform background tasks.	
	*/
	class VisualController
	{
	private:

		GameSimController* m_gsController;

		SkyVisualization m_sky;
		LandscapeVisualizationController m_landscape;
		ObjectVis_Cubical m_objCubical;
		FirstPersonGameCamera m_camera;

		bool m_fullyInited = false;


	public:
		inline VisualController(GameSimController* gsController) :
			m_gsController(gsController),
			m_landscape(gsController),
			m_objCubical(gsController),
			m_sky(gsController)
		{}

		/**!
		Some of the components of the visual controller might choose to start
		resource initialization beforehand, so that it'll be ready for the moment, when actual
		game starts. This method should be called as soon as controller is created to start this initialization
		*/

		void StartBackgroundInit();
		
		/**!
		This method will block untill any background initialization processes is finished. It is also posible to perform foreground
		initialization during this call.
		Service guarantees, that when this method returns, it is ready for work.
		A specific state of a gameSim might be required to perform this step.
		*/
		void WaitForInit();

		/**!
		Updates the states of visualization components for the next frame.
		This method must be called every frame.
		*/
		void Frame();

		~VisualController();


	};
}