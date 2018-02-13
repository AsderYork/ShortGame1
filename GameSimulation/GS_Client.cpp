#include "GS_Client.h"

namespace GEM::GameSim
{
	bool GS_Client::Tick(float Delta)
	{
		//Perform basic simulation tick
		auto Retval = m_gs.Tick(Delta);
		
		
		return Retval;
	}
}