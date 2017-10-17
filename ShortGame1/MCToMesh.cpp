#include "stdafx.h"
#include "MCToMesh.h"

#include <OGRE\OgreMesh2.h>
#include <OGRE\OgreMeshManager2.h>
#include <OGRE\OgreSubMesh2.h>
#include <OGRE\OgreItem.h>

#include <OGRE\OgreMaterialManager.h>
#include <OGRE\OgreMaterial.h>

#include <OGRE\OgreHlms.h>
#include <OGRE\OgreHlmsDatablock.h>
#include <OGRE\Hlms\Pbs\OgreHlmsPbsDatablock.h>
#include <OGRE\OgreHlmsManager.h>

namespace GEM
{
	struct MeshVertices
	{
		float px, py, pz;   //Position
		float nx = 0, ny = 1, nz = 0;   //Normals
		float nu = 0, nv = 0; //Texture Coordinates 1

		MeshVertices() {}
		MeshVertices(float _px, float _py, float _pz,
			float _nx, float _ny, float _nz) :
			px(_px), py(_py), pz(_pz),
			nx(_nx), ny(_ny), nz(_nz) {}

		MeshVertices(float _px, float _py, float _pz) :
			px(_px), py(_py), pz(_pz),
			nx(0), ny(1), nz(0) {}
	};

	Ogre::IndexBufferPacked * MCToMesh::createIndexBuffer(NodesToMCGenerator &Generator)
	{
		Ogre::IndexBufferPacked *indexBuffer = 0;

		Ogre::uint16 *cubeIndices = reinterpret_cast<Ogre::uint16*>(OGRE_MALLOC_SIMD(sizeof(Ogre::uint16) * Generator.getIndexVector().size(),Ogre::MEMCATEGORY_GEOMETRY));

		for (int i = 0; i < Generator.getIndexVector().size(); i++)
		{
			cubeIndices[i] = (Generator.getIndexVector())[i];
		}

		Ogre::RenderSystem *renderSystem = m_ogreService->getRoot()->getRenderSystem();
		Ogre::VaoManager *vaoManager = renderSystem->getVaoManager();

		try
		{
			indexBuffer = vaoManager->createIndexBuffer(Ogre::IndexBufferPacked::IT_16BIT,
				Generator.getIndexVector().size(),
				Ogre::BT_IMMUTABLE,
				cubeIndices, true);
		}
		catch (Ogre::Exception &e)
		{
			// When keepAsShadow = true, the memory will be freed when the index buffer is destroyed.
			// However if for some weird reason there is an exception raised, the memory will
			// not be freed, so it is up to us to do so.
			// The reasons for exceptions are very rare. But we're doing this for correctness.
			OGRE_FREE_SIMD(indexBuffer, Ogre::MEMCATEGORY_GEOMETRY);
			indexBuffer = 0;
			throw e;
		}

		return indexBuffer;
	}

	void MCToMesh::GenerateMesh(NodesToMCGenerator & Generator, int PosX, int PosZ, int Scale)
	{
		Ogre::RenderSystem *renderSystem = m_ogreService->getRoot()->getRenderSystem();
		Ogre::VaoManager *vaoManager = renderSystem->getVaoManager();

		if (Generator.getVertexVector().size() == 0)//Then there is no vertices to draw!
		{
			return;
		}

		Ogre::MeshPtr mesh = Ogre::MeshManager::getSingleton().createManual("MarchingCubies", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		Ogre::SubMesh *subMesh = mesh->createSubMesh();

		//Vertex declaration
		Ogre::VertexElement2Vec vertexElements;
		vertexElements.push_back(Ogre::VertexElement2(Ogre::VET_FLOAT3, Ogre::VES_POSITION));
		vertexElements.push_back(Ogre::VertexElement2(Ogre::VET_FLOAT3, Ogre::VES_NORMAL));
		vertexElements.push_back(Ogre::VertexElement2(Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES));

		MeshVertices *meshVertices = reinterpret_cast<MeshVertices*>(OGRE_MALLOC_SIMD(sizeof(MeshVertices) * Generator.getVertexVector().size(), Ogre::MEMCATEGORY_GEOMETRY));

		//Translate VertexList from Generator to Ogre
		for (int i = 0; i < Generator.getVertexVector().size(); i++)
		{
			meshVertices[i].px = (Generator.getVertexVector())[i]->x;
			meshVertices[i].py = (Generator.getVertexVector())[i]->y;
			meshVertices[i].pz = (Generator.getVertexVector())[i]->z;

			
			meshVertices[i].nx = (Generator.getVertexVector())[i]->nx;
			meshVertices[i].ny = (Generator.getVertexVector())[i]->ny;
			meshVertices[i].nz = (Generator.getVertexVector())[i]->nz;


			//And no Texture cordinates
			//Check for flavors
			if((Generator.getVertexVector())[i]->FlavorUpDown == i)
			{
				meshVertices[i].nu = (Generator.getVertexVector())[i]->uvx;
				meshVertices[i].nv = (Generator.getVertexVector())[i]->uvz;
			}
			else if ((Generator.getVertexVector())[i]->FlavorFrontBack == i)
			{
				meshVertices[i].nu = (Generator.getVertexVector())[i]->uvy;
				meshVertices[i].nv = (Generator.getVertexVector())[i]->uvx;
			}
			else
			{
				meshVertices[i].nu = (Generator.getVertexVector())[i]->uvy;
				meshVertices[i].nv = (Generator.getVertexVector())[i]->uvz;
			}

			
		}

		Ogre::VertexBufferPacked *vertexBuffer = 0;
		try
		{
			//Create the actual vertex buffer.
			vertexBuffer = vaoManager->createVertexBuffer(vertexElements, Generator.getVertexVector().size(), Ogre::BT_IMMUTABLE, meshVertices, false);
		}
		catch (Ogre::Exception &e)
		{
			OGRE_FREE_SIMD(vertexBuffer, Ogre::MEMCATEGORY_GEOMETRY);
			LOGCATEGORY("MCToMesh\GenerateMesh").error("Failed to create VertexBuffer for chunk, in position %i, %i", PosX, PosZ);
			vertexBuffer = 0;
			throw e;
		}

		Ogre::VertexBufferPackedVec vertexBuffers;
		vertexBuffers.push_back(vertexBuffer);

		Ogre::IndexBufferPacked *indexBuffer = createIndexBuffer(Generator); //Create the actual index buffer
		Ogre::VertexArrayObject *vao = vaoManager->createVertexArrayObject(
			vertexBuffers, indexBuffer, Ogre::OT_TRIANGLE_LIST);

		subMesh->mVao[Ogre::VpNormal].push_back(vao);
		//Use the same geometry for shadow casting.
		subMesh->mVao[Ogre::VpShadow].push_back(vao);

		float meshSize = pow(pow(CHUNK_SIZE,2)*2 + pow(CHUNK_HEIGHT, 2), 0.5f);
		mesh->_setBounds(Ogre::Aabb(Ogre::Vector3::ZERO, Ogre::Vector3::UNIT_SCALE*meshSize), false);
		mesh->_setBoundingSphereRadius(meshSize);

		auto SceneManager = m_ogreService->getRoot()->getSceneManager("ExampleSMInstance");

		m_MarchingCubesItem = SceneManager->createItem("MarchingCubies");
		m_MarchingCubeNode = SceneManager->getRootSceneNode(Ogre::SCENE_DYNAMIC)->createChildSceneNode(Ogre::SCENE_DYNAMIC);

		m_MarchingCubeNode->attachObject(m_MarchingCubesItem);
		m_MarchingCubeNode->setPosition(0, 0, 0);

		m_MarchingCubesItem->setDatablock("HlmsPbs1");

	}

	void GEM::MCToMesh::CreateMarchingCubeDatablock()
	{
		/*Ogre::HlmsManager *hlmsManager = m_ogreService->getRoot()->getHlmsManager();
		Ogre::HlmsTextureManager *hlmsTextureManager = hlmsManager->getTextureManager();

		auto hlmsPbs = hlmsManager->getHlms(Ogre::HLMS_PBS);

		Ogre::String datablockName = "MarchingCubesBlob";

		Ogre::HlmsPbsDatablock *datablock;
		datablock = static_cast<Ogre::HlmsPbsDatablock*>(hlmsPbs->getDatablock(datablockName));
		if (datablock == nullptr)
		{
			datablock = static_cast<Ogre::HlmsPbsDatablock*>(
				hlmsPbs->createDatablock(datablockName,
					datablockName,
					Ogre::HlmsMacroblock(),
					Ogre::HlmsBlendblock(),
					Ogre::HlmsParamVec()));

			datablock->setDiffuse(Ogre::Vector3(0.6f, 0.6f, 2.0f));
			//Ogre::HlmsMacroblock macro = *(datablock->getMacroblock());
			//macro.mCullMode = Ogre::CULL_NONE;
			//datablock->setMacroblock(macro);

			//datablock->setTransparency(0.7);

		}*/

	}
}


