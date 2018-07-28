#pragma once
#include "ScreenController.h"
#include "GameSimService.h"
#include "NetworkController.h"
#include "VisualController.h"

#include "PlayerInputEventGenerator.h"

namespace GEM
{
	class MainGameScreen : public Screen
	{
	private:
		GameSimController* m_gsController;
		NetworkController* m_network;

		PlayerInputEventGenerator m_PIEG;
		VisualController m_visual;

		bool m_basicInitPerformed = false;

	public:
		MainGameScreen(NetworkController* network, GameSimController* gs);

		virtual void IsOnTop();

		virtual void Init();

		virtual void PostFrame(float timeDelta) override;
	};
}