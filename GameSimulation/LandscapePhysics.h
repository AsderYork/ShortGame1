#pragma once
#include "LandscapeChunkStorageListener.h"


#include <btBulletDynamicsCommon.h>

#include <memory>

namespace GEM::GameSim
{
	struct PerChunkCollisionObject
	{
		std::unique_ptr<btCollisionShape> collisionShape;
		std::unique_ptr<btDefaultMotionState> motionState;
		std::unique_ptr<btRigidBody> rigidBody;

	};

	/**!
	This class performs phsysics related stuff with loaded map. Such as creating and managing
	Collision Shapes and querries to a landscape.
	*/
	class LandscapePhysics : public ChunkStorageListener<PerChunkCollisionObject>
	{
	public:
		virtual void NewChunkAdded(LandscapeChunk * NewChunk, LandscapeMesh* newMesh, PerChunkCollisionObject * SpecificData) override;
		virtual void ChunkRemoved(LandscapeChunk * NewChunk, LandscapeMesh* newMesh, PerChunkCollisionObject * SpecificData) override;
	};
}