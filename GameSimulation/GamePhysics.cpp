#include "GamePhysics.h"
#include <iostream>


namespace GEM::GameSim
{
	void GamePhysics::Initialize()
	{
		m_broadphase = std::make_unique<btDbvtBroadphase>();
		m_collisionConfiguration = std::make_unique<btDefaultCollisionConfiguration>();
		m_collisionDispatcher = std::make_unique<btCollisionDispatcher>(m_collisionConfiguration.get());
		m_solver = std::make_unique<btSequentialImpulseConstraintSolver>();
		m_world = std::make_unique<btDiscreteDynamicsWorld>(m_collisionDispatcher.get(), m_broadphase.get(), m_solver.get(), m_collisionConfiguration.get());


		m_world->setGravity(btVector3(0, -10, 0));
	}

	void GamePhysics::Tick(GameTime time)
	{
		m_world->stepSimulation(GameTimeToSeconds(time), 10);
	}

	
}
