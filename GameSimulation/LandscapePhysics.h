#pragma once
#include "LandscapeChunkStorage.h"
#include "GamePhysics.h"

#include <memory>

namespace GEM::GameSim
{

	struct PerChunkCollisionObject
	{
		std::unique_ptr<btTriangleIndexVertexArray> m_mesh;
		std::unique_ptr<btDefaultMotionState> motionState;
		std::unique_ptr<btRigidBody> rigidBody;
		std::unique_ptr<btCollisionShape> collisionShape;

	};

	/**!
	This class performs phsysics related stuff with loaded map. Such as creating and managing
	Collision Shapes and querries to a landscape.
	*/
	class LandscapePhysics : public ChunkStorageListener<PerChunkCollisionObject>
	{
	private:
		GamePhysics * m_physics;
	public:
		LandscapePhysics(GamePhysics* Physics) : m_physics(Physics){}
		virtual void NewChunkAdded(LandscapeChunk * NewChunk, LandscapeMesh* newMesh, PerChunkCollisionObject * SpecificData) override;
		virtual void ChunkRemoved(LandscapeChunk * NewChunk, LandscapeMesh* newMesh, PerChunkCollisionObject * SpecificData) override;
	};
}