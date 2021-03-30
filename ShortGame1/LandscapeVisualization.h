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


		LandscapeVisualMesh(const LandscapeVisualMesh& other) = delete;

		inline LandscapeVisualMesh() {}
		inline LandscapeVisualMesh(LandscapeVisualMesh && other) :
			marchingCubesItem(other.marchingCubesItem),
			marchingCubeNode(other.marchingCubeNode),
			mesh(std::move(other.mesh))
		{

			other.marchingCubesItem = nullptr;
			other.marchingCubeNode = nullptr;
		}
		inline LandscapeVisualMesh & operator=(LandscapeVisualMesh && other)
		{
			Clear(); //Who knew, when you move something in to somethoing, the thing you move into is supposed to get invalidated!
			marchingCubesItem = other.marchingCubesItem;
			marchingCubeNode = other.marchingCubeNode;
			mesh = std::move(other.mesh);
			other.marchingCubesItem = nullptr;
			other.marchingCubeNode = nullptr;
			return *this;
		}

		~LandscapeVisualMesh() {
			Clear();
		}

		/*inline LandscapeVisualMesh& operator=(const LandscapeVisualMesh&& other)
		{
			marchingCubesItem = other.marchingCubesItem;
			marchingCubeNode = other.marchingCubeNode;
			mesh = std::move(other.mesh);
			return *this;
		}*/

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


		static LandscapeVisualMesh GenerateVisualMesh(GameSim::LandscapeMesh* Chunk, GameSim::LandscapeChunk* ChunkData);

		static LandscapeVisualMesh DoCube(float x = 0.0f, float y = 20.0f, float z = 0.0f, Ogre::Vector3 colour = Ogre::Vector3(1.0f, 1.0f, 1.0f), float transperency = 1.0f, Ogre::Vector3 scale = Ogre::Vector3(1.0f));
	
		static LandscapeVisualMesh GenerateMeshFromVertices(std::vector<Ogre::Vector3> vertices, std::vector<UINT> indices, Ogre::Vector3 originPos, int size);
	
	};
}