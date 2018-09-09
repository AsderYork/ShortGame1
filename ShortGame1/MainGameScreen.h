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
		
		virtual bool Init();

		virtual void PostFrame(float timeDelta) override;

		// Унаследовано через Screen
		virtual void OnTop() override;
		virtual void NoLongerOnTop() override;
		virtual void LeaveStack() override;
	};
}