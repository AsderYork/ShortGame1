#pragma once
#include <LandscapeMeshGenerator.h>
#include "Ogre_Service.h"

namespace GEM
{
	/**!
	Builds Ogre-compatable mesh from LandscapeMeshGenerator
	*/
	class LandscapeVisualization
	{
	private:
		Ogre_Service * m_ogreService;
		const GameSim::LandscapeMeshGenerator* m_generator;

		Ogre::Item* m_marchingCubesItem = nullptr;
		Ogre::SceneNode* m_marchingCubeNode = nullptr;
		Ogre::MeshPtr m_mesh;

		Ogre::IndexBufferPacked* createIndexBuffer();


	public:
		LandscapeVisualization(Ogre_Service* OgreService, const GameSim::LandscapeMeshGenerator* Chunk)
			: m_ogreService(OgreService), m_generator(Chunk) {}

		~LandscapeVisualization();
		void GenerateMesh();
		/**
		Removes mesh, if it's exist
		*/
		void RemoveMesh();


		void DoCube();
		void UndoCube();
	};
}