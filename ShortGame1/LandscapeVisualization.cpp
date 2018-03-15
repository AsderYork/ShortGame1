#include "stdafx.h"
#include "LandscapeVisualization.h"

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
		float px = 0, py = 0, pz = 0;   //Position
		float nx = 0, ny = 1, nz = 0;   //Normals
		float nu = 0, nv = 0; //Texture Coordinates 1
		float TBR = 1.0f, TBG = 0.0f, TBB = 0.0f, TBA = 0.0f;

		MeshVertices() {}
	};

	void LandscapeVisualization::GenerateMesh()
	{
		Ogre::RenderSystem *renderSystem = m_ogreService->getRoot()->getRenderSystem();
		Ogre::VaoManager *vaoManager = renderSystem->getVaoManager();
		auto SceneManager = m_ogreService->getRoot()->getSceneManager("ExampleSMInstance");

		auto [ChunkPosX, ChunkPosZ] = m_generator->getPos();
		std::string MeshName = "MarchingCubies" + std::to_string(ChunkPosX) + "X" + std::to_string(ChunkPosZ);

		//Remove previously created mesh if it's exist
		if (m_marchingCubesItem != nullptr)
		{
			m_marchingCubesItem->detachFromParent();
			SceneManager->destroyItem(m_marchingCubesItem);
		}
		if (m_mesh != nullptr)
		{
			Ogre::MeshManager::getSingleton().remove(m_mesh);
		}


		if (m_generator->getVertices().size() == 0)//Then there is no vertices to draw!
		{return;}

		m_mesh = Ogre::MeshManager::getSingleton().createManual(MeshName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		Ogre::SubMesh *subMesh = m_mesh->createSubMesh();

		Ogre::VertexElement2Vec vertexElements;
		vertexElements.push_back(Ogre::VertexElement2(Ogre::VET_FLOAT3, Ogre::VES_POSITION));
		vertexElements.push_back(Ogre::VertexElement2(Ogre::VET_FLOAT3, Ogre::VES_NORMAL));
		vertexElements.push_back(Ogre::VertexElement2(Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES));
		vertexElements.push_back(Ogre::VertexElement2(Ogre::VET_FLOAT4, Ogre::VES_SPECULAR));
		
		MeshVertices *meshVertices = reinterpret_cast<MeshVertices*>(OGRE_MALLOC_SIMD(sizeof(MeshVertices) * m_generator->getVertices().size(), Ogre::MEMCATEGORY_GEOMETRY));


		//Translate VertexList from Generator to Ogre
		for (int i = 0; i < m_generator->getVertices().size(); i++)
		{
			auto& Vertex = m_generator->getVertices()[i];

			meshVertices[i].px = static_cast<float>(Vertex.pos.x()) + ChunkPosX* GameSim::LandscapeChunk_Size;
			meshVertices[i].py = static_cast<float>(Vertex.pos.y());
			meshVertices[i].pz = static_cast<float>(Vertex.pos.z()) + ChunkPosZ* GameSim::LandscapeChunk_Size;


			Vertex.normal.normalize();
			meshVertices[i].nx = static_cast<float>(Vertex.normal.x());
			meshVertices[i].ny = static_cast<float>(Vertex.normal.y());
			meshVertices[i].nz = static_cast<float>(Vertex.normal.z());

			meshVertices[i].nu = static_cast<float>(Vertex.pos.x());
			meshVertices[i].nv = static_cast<float>(Vertex.pos.z());

		}


		Ogre::VertexBufferPacked *vertexBuffer = 0;
		try
		{
			//Create the actual vertex buffer.
			vertexBuffer = vaoManager->createVertexBuffer(vertexElements, m_generator->getVertices().size(), Ogre::BT_IMMUTABLE, meshVertices, false);
		}
		catch (Ogre::Exception &e)
		{
			OGRE_FREE_SIMD(vertexBuffer, Ogre::MEMCATEGORY_GEOMETRY);
			LOGCATEGORY("MCToMesh/GenerateMesh").error("Failed to create VertexBuffer for chunk, in position %i, %i", ChunkPosX, ChunkPosZ);
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

		float meshSize = GameSim::LandscapeChunk_Size / 2;
		float meshRadius = (float)pow(((GameSim::LandscapeChunk_Size*GameSim::LandscapeChunk_Size) / 2) + ((GameSim::LandscapeChunk_Height*GameSim::LandscapeChunk_Height) / 4), 0.5f);
		m_mesh->_setBounds(Ogre::Aabb(
			Ogre::Vector3::ZERO + Ogre::Vector3(ChunkPosX + meshSize, GameSim::LandscapeChunk_Height / 2, ChunkPosZ + meshSize),
			Ogre::Vector3(GameSim::LandscapeChunk_Size / 2, GameSim::LandscapeChunk_Height / 2, GameSim::LandscapeChunk_Size / 2)),
			false);
		m_mesh->_setBoundingSphereRadius(meshRadius);

		//subMesh->setMaterialName("HlmsPbs1");



		m_marchingCubesItem = SceneManager->createItem(MeshName);
		m_marchingCubeNode = SceneManager->getRootSceneNode(Ogre::SCENE_DYNAMIC)->createChildSceneNode(Ogre::SCENE_DYNAMIC);

		m_marchingCubeNode->attachObject(m_marchingCubesItem);
		m_marchingCubeNode->setPosition(0, 0, 0);

	}

	Ogre::IndexBufferPacked * LandscapeVisualization::createIndexBuffer()
	{
		Ogre::IndexBufferPacked *indexBuffer = 0;

		Ogre::uint16 *cubeIndices = reinterpret_cast<Ogre::uint16*>(OGRE_MALLOC_SIMD(sizeof(Ogre::uint16) * m_generator->getInidces().size(), Ogre::MEMCATEGORY_GEOMETRY));

		/**!
		No index can be bigger then 2^16
		*/
		assert(m_generator->getVertices().size() < std::numeric_limits<Ogre::uint16>::max());

		for (int i = 0; i < m_generator->getInidces().size(); i++)
		{
			cubeIndices[i] = static_cast<Ogre::uint16>(m_generator->getInidces()[i]);
		}

		Ogre::RenderSystem *renderSystem = m_ogreService->getRoot()->getRenderSystem();
		Ogre::VaoManager *vaoManager = renderSystem->getVaoManager();

		try
		{
			indexBuffer = vaoManager->createIndexBuffer(Ogre::IndexBufferPacked::IT_16BIT,
				m_generator->getInidces().size(),
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

	void LandscapeVisualization::RemoveMesh()
	{
		if (m_marchingCubesItem != nullptr)
		{
			m_marchingCubeNode->removeAndDestroyAllChildren();
			auto SceneManager = m_ogreService->getRoot()->getSceneManager("ExampleSMInstance");
			SceneManager->destroyItem(m_marchingCubesItem);
			m_marchingCubesItem = nullptr;
		}
		if (m_mesh != nullptr)
		{
			Ogre::MeshManager::getSingleton().remove(m_mesh);
			m_mesh.setNull();
		}
	}

	void LandscapeVisualization::DoCube()
	{
		auto mSceneMgr = m_ogreService->getRoot()->getSceneManager("ExampleSMInstance");
		item = mSceneMgr->createItem("Cubexa.mesh");

		node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
		node->attachObject(item);
		node->setPosition(0, 0, 0);
	}

	void LandscapeVisualization::UndoCube()
	{
		if (node == nullptr) { return; }
		node->removeAndDestroyAllChildren();
		m_ogreService->getRoot()->getSceneManager("ExampleSMInstance")->destroyItem(item);
	}

	LandscapeVisualization::~LandscapeVisualization()
	{
		RemoveMesh();

	}
}