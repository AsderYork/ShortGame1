#pragma once
#include "ScreenController.h"
#include "GameSimService.h"
#include "NetworkController.h"

#include "PlayerInputEventGenerator.h"

namespace GEM
{
	class MainGameScreen : public Screen
	{
	private:
		GameSimController* m_gsController;
		NetworkController* m_network;
		PlayerInputEventGenerator m_PIEG;
	public:
			MainGameScreen(NetworkController* network, GameSimController* gs) : m_network(network), m_gsController(gs) {}

		virtual void PostFrame(float timeDelta) override;
	};
}