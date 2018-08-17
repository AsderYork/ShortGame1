#pragma once
#include <GameTime.h>
#include <GameSimulation.h>

#include <Caelum/Caelum.h>
#include <memory>

namespace GEM
{
	/**!
	Visualizes sky. Sun, moon, stars e.t.c. Maybe later there will be clouds and other weather effects
	*/
	class SkyVisualization
	{
	private:

		GameSim::GameTime m_lastGameTime = 0;
		GameSim::GameSimulation* m_gs;
		std::unique_ptr<Caelum::CaelumSystem> m_caelumSystem;

		float m_timeOfDayCoeff;
		bool m_oneFrameSkipped = false;//Caelum can't work properly in a \c frame when it's created, so we're just skip one.

		void Shutdown();
		

	public:

		/**!
		Requires GameSim to know the time
		\param[in] timeOfDayCoeff Determines how many game-day passes in one real day
		*/
		inline SkyVisualization(GameSim::GameSimulation* gs, float timeOfDayCoeff = 100.0f) : m_gs(gs), m_timeOfDayCoeff(timeOfDayCoeff) {}

		/**!
		SkyVisualization depends on a WorldTime object which might not be avaliable at the beginning.
		But it should be created after the first package from the server! This method must be as soon as
		WorldTime object is created.
		*/
		void StartVisualization();

		void frame();
	};
}