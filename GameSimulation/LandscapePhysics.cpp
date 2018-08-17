#include "LandscapePhysics.h"
#include "LandscapeMeshGenerator.h"
#include "Mixin_Movable Singleton.h"

#include <btBulletDynamicsCommon.h>

namespace GEM::GameSim
{
	LandscapePhysics::LandscapePhysics(GamePhysics * Physics) : m_physics(Physics)
	{
		Mixin_Movable_Singleton::Instance().SetLandscapePhysicsPtr(this);
	}
	void LandscapePhysics::NewChunkAdded(LandscapeChunk * NewChunk, LandscapeMesh* newMesh, PerChunkCollisionObject * SpecificData)
	{
		SpecificData->m_mesh = std::make_unique<btTriangleIndexVertexArray>(
			static_cast<int>(newMesh->m_triangles.size()),
			(int*)newMesh->m_indices.data(),
			static_cast<int>(3 * sizeof(uint32_t)),
			static_cast<int>(newMesh->m_vertices.size()),
			(btScalar*)newMesh->m_vertices.data(),
			static_cast<int>(sizeof(VertexType)));

		SpecificData->collisionShape = std::make_unique<btBvhTriangleMeshShape>(SpecificData->m_mesh.get(), true);

		auto[PosX, PosZ] = NewChunk->getPosition();
		SpecificData->motionState =  
			std::make_unique<btDefaultMotionState>(
				btTransform(btQuaternion(0, 0, 0, 1),
					btVector3(static_cast<btScalar>(PosX*(int)LandscapeChunk_Size),
					static_cast<btScalar>(0),
					static_cast<btScalar>(PosZ*(int)LandscapeChunk_Size)
					)
				)
			);

		btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, SpecificData->motionState.get(), SpecificData->collisionShape.get(), btVector3(0, 0, 0));


		SpecificData->rigidBody = std::make_unique<btRigidBody>(groundRigidBodyCI);

		m_physics->getWorld()->addRigidBody(SpecificData->rigidBody.get());

	}

	void LandscapePhysics::ChunkRemoved(LandscapeChunk * NewChunk, LandscapeMesh* newMesh, PerChunkCollisionObject * SpecificData)
	{
		if (SpecificData->rigidBody)
		{		
			m_physics->getWorld()->removeRigidBody(SpecificData->rigidBody.get());
		}
	}

	std::optional<btVector3> LandscapePhysics::RayTest(btVector3 start, btVector3 onLine)
	{
		btCollisionWorld::ClosestRayResultCallback RayCallback(start, onLine);

		float Height = 20.0f;
		m_physics->getWorld()->rayTest(start, onLine, RayCallback);
		if (RayCallback.hasHit())
		{
			return RayCallback.m_hitPointWorld;
		}
		return std::nullopt;
	}

}
