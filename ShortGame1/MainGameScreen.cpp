#include "stdafx.h"
#include "MainGameScreen.h"
#include "LoginScreen.h"

namespace GEM
{
	void MainGameScreen::PostFrame(float timeDelta)
	{
		if (!m_gsController->getSimulationState())
		{
			m_network->Disconnect();
			LOGCATEGORY("MainGameScreen/PostFrame").info("Simmulation is stopped!");
			getController()->AddScreen<LoginScreen>(m_network, m_gsController, "Simulation were terminated. Try to reconnect");
			Finish();
		}
		
	}
}