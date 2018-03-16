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
		GameSim::LandscapeMeshGenerator* m_generator;

		Ogre::Item* m_marchingCubesItem = nullptr;
		Ogre::SceneNode* m_marchingCubeNode = nullptr;
		Ogre::MeshPtr m_mesh;

		Ogre::IndexBufferPacked* createIndexBuffer(std::vector<uint16_t>& Indices);

		Ogre::Item* item = nullptr;
		Ogre::SceneNode* node = nullptr;

		/**!
		Helper structure. Used to add orientation to a triangles in a mesh to allow
		texturing.
		*/
		struct OrientHolder {
			uint16_t OriginalVertex;

			uint16_t UpDownPos, LeftRightPos, FrontBackPos;

			enum Presentation : char { UNDEF, UPDOWN, LEFTRIGHT, FRONTBACK }
			currentPresentation;

			inline uint16_t& accessPos(Presentation Orient)
			{
				switch (Orient)
				{
				case UPDOWN: {return UpDownPos; }
				case LEFTRIGHT: {return LeftRightPos; }
				case FRONTBACK: {return FrontBackPos; }
				}
			}

			OrientHolder() : OriginalVertex(std::numeric_limits<uint16_t>::max()), currentPresentation(UNDEF),
				UpDownPos(std::numeric_limits<uint16_t>::max()),
				LeftRightPos(std::numeric_limits<uint16_t>::max()),
				FrontBackPos(std::numeric_limits<uint16_t>::max()) {}

			OrientHolder(uint16_t Orv, Presentation orient) : OriginalVertex(Orv), currentPresentation(orient),
				UpDownPos(std::numeric_limits<uint16_t>::max()),
				LeftRightPos(std::numeric_limits<uint16_t>::max()),
				FrontBackPos(std::numeric_limits<uint16_t>::max()) {}

		};


		std::pair<std::vector<OrientHolder>, std::vector<uint16_t>> processTriangles();


	public:
		LandscapeVisualization(Ogre_Service* OgreService, GameSim::LandscapeMeshGenerator* Chunk)
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