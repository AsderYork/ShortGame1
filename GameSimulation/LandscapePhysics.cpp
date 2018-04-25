#include "LandscapePhysics.h"
#include "LandscapeMeshGenerator.h"

#include <btBulletDynamicsCommon.h>

namespace GEM::GameSim
{

	void LandscapePhysics::NewChunkAdded(LandscapeChunk * NewChunk, LandscapeMesh* newMesh, PerChunkCollisionObject * SpecificData)
	{
		SpecificData->m_mesh = std::make_unique<btTriangleIndexVertexArray>(
			newMesh->m_triangles.size(),
			(int*)newMesh->m_indices.data(),
			3 * sizeof(uint32_t),
			newMesh->m_vertices.size(),
			(btScalar*)newMesh->m_vertices.data(),
			sizeof(VertexType));

		SpecificData->collisionShape = std::make_unique<btBvhTriangleMeshShape>(SpecificData->m_mesh.get(), true);

		auto[PosX, PosZ] = NewChunk->getPosition();
		SpecificData->motionState =  std::make_unique<btDefaultMotionState>(btTransform(btQuaternion(0, 0, 0, 1), btVector3(PosX*LandscapeChunk_Size, 0, PosZ*LandscapeChunk_Size)));

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

}
