#pragma once
#include <LandscapeMeshGenerator.h>
#include "Ogre_Service.h"

namespace GEM
{
	struct LandscapeVisualMesh
	{
		Ogre::Item* marchingCubesItem = nullptr;
		Ogre::SceneNode* marchingCubeNode = nullptr;
		Ogre::MeshPtr mesh;

		inline LandscapeVisualMesh() {}
		inline LandscapeVisualMesh(LandscapeVisualMesh && other) :
			marchingCubesItem(other.marchingCubesItem),
			marchingCubeNode(other.marchingCubeNode),
			mesh(std::move(other.mesh))
		{}
		inline LandscapeVisualMesh & operator=(LandscapeVisualMesh && other)
		{
			marchingCubesItem = other.marchingCubesItem;
			marchingCubeNode = other.marchingCubeNode;
			mesh = std::move(other.mesh);
			return *this;
		}

		void Clear();

	};
	/**!
	Builds Ogre-compatable mesh from LandscapeMeshGenerator
	*/
	class LandscapeVisualization
	{
	private:
		static Ogre::IndexBufferPacked* createIndexBuffer(std::vector<uint16_t>& Indices);


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


		static std::pair<std::vector<OrientHolder>, std::vector<uint16_t>> processTriangles(GameSim::LandscapeMesh* Chunk);


	public:


		static LandscapeVisualMesh GenerateVisualMesh(GameSim::LandscapeMesh* Chunk, std::pair<int, int> ChunkPos);

		static LandscapeVisualMesh DoCube();
	};
}