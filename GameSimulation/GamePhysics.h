#pragma once
#include "GameTime.h"

#include <btBulletDynamicsCommon.h>
#include <memory>


namespace GEM::GameSim
{
	/**!
	Responsible for providing interfaces for creating physics objects.
	*/
	class GamePhysics
	{
	private:
		std::unique_ptr<btBroadphaseInterface> m_broadphase;
		std::unique_ptr<btDefaultCollisionConfiguration> m_collisionConfiguration;
		std::unique_ptr<btCollisionDispatcher> m_collisionDispatcher;
		std::unique_ptr<btSequentialImpulseConstraintSolver> m_solver;
		std::unique_ptr<btDiscreteDynamicsWorld> m_world;

	public:

		inline btDiscreteDynamicsWorld* getWorld() { return m_world.get(); }
		void Initialize();
		~GamePhysics();

		void Tick(GameTime time);
	};
}