#pragma once
#include "Ogre_Service.h"
#include "NodesToMCGenerator.h"

namespace GEM
{
	/**!
	Builds a mesh from Lists that MCGenerator creates
	*/
	class MCToMesh
	{
	private:
		Ogre_Service* m_ogreService;
		Ogre::Item* m_MarchingCubesItem = nullptr;
		Ogre::SceneNode* m_MarchingCubeNode = nullptr;

		Ogre::IndexBufferPacked* createIndexBuffer(NodesToMCGenerator &Generator);

	public:
		MCToMesh(Ogre_Service* ogreService) : m_ogreService(ogreService) {}

		/**!
		Creates a mesh from Vertex and Index vector calculated by specified generator.
		
		\param[in] Generator Generator that holds calculated vectors
		\param[in] PosX X-coordinate of (0,0) corner of a chunk
		\param[in] PosZ Z-coordinate of (0,0) corner of a chunk
		\param[in] Scale Scale of a chunk across X,Y and Z axis
		*/
		void GenerateMesh(NodesToMCGenerator &Generator, int PosX, int PosZ, int Scale);
	};
}