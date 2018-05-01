#include "Mixin_Movable Singleton.h"

namespace GEM::GameSim
{
	void Mixin_Movable_Singleton::SetLandscapePhysicsPtr(LandscapePhysics * PhysPtr)
	{
		Instance().m_landPhys = PhysPtr;
	}
}
