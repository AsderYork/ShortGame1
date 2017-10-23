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
		Ogre::MeshPtr m_mesh;

		Ogre::IndexBufferPacked* createIndexBuffer();

		int m_posX;
		int m_posZ;
		int m_scale;

		NTMCG_Base* m_generator = nullptr;


	public:
		MCToMesh(Ogre_Service* ogreService, NTMCG_Base* Generator, int PosX, int PosZ, int Scale) :
			m_ogreService(ogreService),
			m_generator(Generator),
			m_posX(PosX),
			m_posZ(PosZ),
			m_scale(Scale)
			{}

		~MCToMesh();

		/**!
		Creates a mesh from Vertex and Index vector calculated by specified generator.
		
		\param[in] Generator Generator that holds calculated vectors
		\param[in] PosX X-coordinate of (0,0) corner of a chunk
		\param[in] PosZ Z-coordinate of (0,0) corner of a chunk
		\param[in] Scale Scale of a chunk across X,Y and Z axis
		*/
		void GenerateMesh();

	};
}