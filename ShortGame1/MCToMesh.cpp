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

	Ogre::IndexBufferPacked * MCToMesh::createIndexBuffer()
	{
		Ogre::IndexBufferPacked *indexBuffer = 0;

		Ogre::uint16 *cubeIndices = reinterpret_cast<Ogre::uint16*>(OGRE_MALLOC_SIMD(sizeof(Ogre::uint16) * m_generator->getIndexVectorSize(),Ogre::MEMCATEGORY_GEOMETRY));

		for (int i = 0; i < m_generator->getIndexVectorSize(); i++)
		{
			cubeIndices[i] = m_generator->getIndexVectorElement(i);
		}

		Ogre::RenderSystem *renderSystem = m_ogreService->getRoot()->getRenderSystem();
		Ogre::VaoManager *vaoManager = renderSystem->getVaoManager();

		try
		{
			indexBuffer = vaoManager->createIndexBuffer(Ogre::IndexBufferPacked::IT_16BIT,
				m_generator->getIndexVectorSize(),
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

	void MCToMesh::GenerateMesh()
	{
		Ogre::RenderSystem *renderSystem = m_ogreService->getRoot()->getRenderSystem();
		Ogre::VaoManager *vaoManager = renderSystem->getVaoManager();
		auto SceneManager = m_ogreService->getRoot()->getSceneManager("ExampleSMInstance");

		std::string MeshName = "MarchingCubies" + std::to_string(m_posX) + "X" + std::to_string(m_posZ);

		if (m_MarchingCubesItem != nullptr)
		{
			m_MarchingCubesItem->detachFromParent();
			SceneManager->destroyItem(m_MarchingCubesItem);
		}
		Ogre::MeshPtr mesh = Ogre::MeshManager::getSingleton().getByName(MeshName);
		if (mesh != nullptr)
		{
			Ogre::MeshManager::getSingleton().remove(mesh);
		}

		if (m_generator->getVertexVectorSize() == 0)//Then there is no vertices to draw!
		{
			return;
		}

		

		mesh = Ogre::MeshManager::getSingleton().createManual(MeshName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		Ogre::SubMesh *subMesh = mesh->createSubMesh();

		//Vertex declaration
		Ogre::VertexElement2Vec vertexElements;
		vertexElements.push_back(Ogre::VertexElement2(Ogre::VET_FLOAT3, Ogre::VES_POSITION));
		vertexElements.push_back(Ogre::VertexElement2(Ogre::VET_FLOAT3, Ogre::VES_NORMAL));
		vertexElements.push_back(Ogre::VertexElement2(Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES));

		MeshVertices *meshVertices = reinterpret_cast<MeshVertices*>(OGRE_MALLOC_SIMD(sizeof(MeshVertices) * m_generator->getVertexVectorSize(), Ogre::MEMCATEGORY_GEOMETRY));

		//Translate VertexList from Generator to Ogre
		for (int i = 0; i < m_generator->getVertexVectorSize(); i++)
		{
			meshVertices[i].px =  m_generator->getVertexVectorElement(i)->x + m_posX;
			meshVertices[i].py =  m_generator->getVertexVectorElement(i)->y;
			meshVertices[i].pz = m_generator->getVertexVectorElement(i)->z + m_posZ;

			
			meshVertices[i].nx = m_generator->getVertexVectorElement(i)->nx;
			meshVertices[i].ny = m_generator->getVertexVectorElement(i)->ny;
			meshVertices[i].nz = m_generator->getVertexVectorElement(i)->nz;


			//And no Texture cordinates
			//Check for flavors
			/*if((m_generator->getVertexVector())[i]->FlavorUpDown == i)
			{
				meshVertices[i].nu = (m_generator->getVertexVector())[i]->uvx;
				meshVertices[i].nv = (m_generator->getVertexVector())[i]->uvz;
			}
			else if ((m_generator->getVertexVector())[i]->FlavorFrontBack == i)
			{
				meshVertices[i].nu = (m_generator->getVertexVector())[i]->uvy;
				meshVertices[i].nv = (m_generator->getVertexVector())[i]->uvx;
			}
			else
			{
				meshVertices[i].nu = (m_generator->getVertexVector())[i]->uvy;
				meshVertices[i].nv = (m_generator->getVertexVector())[i]->uvz;
			}*/

			//This one in case if NaiveGenerator
			switch (m_generator->getVertexVectorElement(i)->flavor)
			{
				case NodeToMCGeneratorNaive::MidPointBase::FLAVOR_UPDOWN:
				{
					meshVertices[i].nu = m_generator->getVertexVectorElement(i)->uvx;
					meshVertices[i].nv = m_generator->getVertexVectorElement(i)->uvz;
					break;
				}

				case NodeToMCGeneratorNaive::MidPointBase::FLAVOR_FRONTBACK:
				{
					meshVertices[i].nu = m_generator->getVertexVectorElement(i)->uvy;
					meshVertices[i].nv = m_generator->getVertexVectorElement(i)->uvx;
					break;
				}

				case NodeToMCGeneratorNaive::MidPointBase::FLAVOR_LEFTRIGHT:
				{
					meshVertices[i].nu = m_generator->getVertexVectorElement(i)->uvy;
					meshVertices[i].nv = m_generator->getVertexVectorElement(i)->uvz;
					break;
				}
			}
			
		}

		Ogre::VertexBufferPacked *vertexBuffer = 0;
		try
		{
			//Create the actual vertex buffer.
			vertexBuffer = vaoManager->createVertexBuffer(vertexElements, m_generator->getVertexVectorSize(), Ogre::BT_IMMUTABLE, meshVertices, false);
		}
		catch (Ogre::Exception &e)
		{
			OGRE_FREE_SIMD(vertexBuffer, Ogre::MEMCATEGORY_GEOMETRY);
			LOGCATEGORY("MCToMesh\GenerateMesh").error("Failed to create VertexBuffer for chunk, in position %i, %i", m_posX, m_posZ);
			vertexBuffer = 0;
			throw e;
		}

		Ogre::VertexBufferPackedVec vertexBuffers;
		vertexBuffers.push_back(vertexBuffer);

		Ogre::IndexBufferPacked *indexBuffer = createIndexBuffer(); //Create the actual index buffer
		Ogre::VertexArrayObject *vao = vaoManager->createVertexArrayObject(
			vertexBuffers, indexBuffer, Ogre::OT_TRIANGLE_LIST);

		subMesh->mVao[Ogre::VpNormal].push_back(vao);
		//Use the same geometry for shadow casting.
		subMesh->mVao[Ogre::VpShadow].push_back(vao);

		float meshSize = pow(pow(CHUNK_SIZE,2)*2 + pow(CHUNK_HEIGHT, 2), 0.5f);
		mesh->_setBounds(Ogre::Aabb(Ogre::Vector3::ZERO, Ogre::Vector3::UNIT_SCALE*meshSize), false);
		mesh->_setBoundingSphereRadius(meshSize);

		

		

		m_MarchingCubesItem = SceneManager->createItem("MarchingCubies" + std::to_string(m_posX) + "X" + std::to_string(m_posZ));
		m_MarchingCubeNode = SceneManager->getRootSceneNode(Ogre::SCENE_DYNAMIC)->createChildSceneNode(Ogre::SCENE_DYNAMIC);

		m_MarchingCubeNode->attachObject(m_MarchingCubesItem);
		m_MarchingCubeNode->setPosition(0, 0, 0);

		m_MarchingCubesItem->setDatablock("HlmsPbs1");

	}

}


