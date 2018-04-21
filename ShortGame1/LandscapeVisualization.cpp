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
		float TBR = 1.0f, TBG = 0.5f, TBB = 0.5f, TBA = 1.0f;

		MeshVertices() {}
	};

	LandscapeVisualMesh LandscapeVisualization::GenerateVisualMesh(GameSim::LandscapeMesh* Chunk, std::pair<int, int> ChunkPos)
	{
		auto[ChunkPosX, ChunkPosZ] = ChunkPos;
		Ogre::RenderSystem *renderSystem = Ogre::Root::getSingleton().getRenderSystem();
		Ogre::VaoManager *vaoManager = renderSystem->getVaoManager();
		auto SceneManager = Ogre::Root::getSingleton().getSceneManager("ExampleSMInstance");

		std::string MeshName = "MarchingCubies" + std::to_string(ChunkPosX) + "X" + std::to_string(ChunkPosZ);

		LandscapeVisualMesh ReturnMesh;
		if (Chunk->m_vertices.size() == 0)//Then there is no vertices to draw!
		{return ReturnMesh; }

		ReturnMesh.mesh = Ogre::MeshManager::getSingleton().createManual(MeshName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		Ogre::SubMesh *subMesh = ReturnMesh.mesh->createSubMesh();

		Ogre::VertexElement2Vec vertexElements;
		vertexElements.push_back(Ogre::VertexElement2(Ogre::VET_FLOAT3, Ogre::VES_POSITION));
		vertexElements.push_back(Ogre::VertexElement2(Ogre::VET_FLOAT3, Ogre::VES_NORMAL));
		vertexElements.push_back(Ogre::VertexElement2(Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES));
		vertexElements.push_back(Ogre::VertexElement2(Ogre::VET_FLOAT4, Ogre::VES_SPECULAR));
		
		auto[Orientations, NewIndices] = processTriangles(Chunk);

		MeshVertices *meshVertices = reinterpret_cast<MeshVertices*>(OGRE_MALLOC_SIMD(sizeof(MeshVertices) * Orientations.size(), Ogre::MEMCATEGORY_GEOMETRY));


		//Translate VertexList from Generator to Ogre
		for (int i = 0; i < Orientations.size(); i++)
		{
			auto& Vertex = Chunk->m_vertices[Orientations[i].OriginalVertex];

			meshVertices[i].px = static_cast<float>(Vertex.pos.x());
			meshVertices[i].py = static_cast<float>(Vertex.pos.y());
			meshVertices[i].pz = static_cast<float>(Vertex.pos.z());


			Vertex.normal.normalize();
			meshVertices[i].nx = static_cast<float>(Vertex.normal.x());
			meshVertices[i].ny = static_cast<float>(Vertex.normal.y());
			meshVertices[i].nz = static_cast<float>(Vertex.normal.z());

			switch (Orientations[i].currentPresentation)
			{
			case OrientHolder::UPDOWN: {
				meshVertices[i].nu = static_cast<float>(Vertex.pos.x());
				meshVertices[i].nv = static_cast<float>(Vertex.pos.z());
				break; }
			case OrientHolder::LEFTRIGHT: {
				meshVertices[i].nu = static_cast<float>(Vertex.pos.y());
				meshVertices[i].nv = static_cast<float>(Vertex.pos.z());
				break; }
			case OrientHolder::FRONTBACK: {
				meshVertices[i].nu = static_cast<float>(Vertex.pos.y());
				meshVertices[i].nv = static_cast<float>(Vertex.pos.x());
				break; }
			}

			meshVertices[i].TBR = 1.0;
			meshVertices[i].TBG = 0.0;
			meshVertices[i].TBB = 0.0;

		}


		Ogre::VertexBufferPacked *vertexBuffer = 0;
		try
		{
			//Create the actual vertex buffer.
			vertexBuffer = vaoManager->createVertexBuffer(vertexElements, Orientations.size(), Ogre::BT_IMMUTABLE, meshVertices, false);
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

		Ogre::IndexBufferPacked *indexBuffer = createIndexBuffer(NewIndices); //Create the actual index buffer
		Ogre::VertexArrayObject *vao = vaoManager->createVertexArrayObject(
			vertexBuffers, indexBuffer, Ogre::OT_TRIANGLE_LIST);

		subMesh->mVao[Ogre::VpNormal].push_back(vao);
		//Use the same geometry for shadow casting.
		subMesh->mVao[Ogre::VpShadow].push_back(vao);

		float meshSize = GameSim::LandscapeChunk_Size / 2;
		float meshRadius = (float)pow(((GameSim::LandscapeChunk_Size*GameSim::LandscapeChunk_Size) / 2) + ((GameSim::LandscapeChunk_Height*GameSim::LandscapeChunk_Height) / 4), 0.5f);
		ReturnMesh.mesh->_setBounds(Ogre::Aabb(
			Ogre::Vector3::ZERO + Ogre::Vector3(ChunkPosX + meshSize, GameSim::LandscapeChunk_Height / 2, ChunkPosZ + meshSize),
			Ogre::Vector3(GameSim::LandscapeChunk_Size / 2, GameSim::LandscapeChunk_Height / 2, GameSim::LandscapeChunk_Size / 2)),
			false);
		ReturnMesh.mesh->_setBoundingSphereRadius(meshRadius);

		subMesh->setMaterialName("HlmsPbs1");



		ReturnMesh.marchingCubesItem = SceneManager->createItem(MeshName);
		ReturnMesh.marchingCubeNode = SceneManager->getRootSceneNode(Ogre::SCENE_DYNAMIC)->createChildSceneNode(Ogre::SCENE_DYNAMIC);

		ReturnMesh.marchingCubeNode->attachObject(ReturnMesh.marchingCubesItem);
		ReturnMesh.marchingCubeNode->setPosition(ChunkPosX * (int)GameSim::LandscapeChunk_Size, 0, ChunkPosZ * (int)GameSim::LandscapeChunk_Size);
		return ReturnMesh;
	}

	Ogre::IndexBufferPacked * LandscapeVisualization::createIndexBuffer(std::vector<uint16_t>& Indices)
	{
		Ogre::IndexBufferPacked *indexBuffer = 0;


		Ogre::uint16 *cubeIndices = reinterpret_cast<Ogre::uint16*>(OGRE_MALLOC_SIMD(sizeof(Ogre::uint16) * Indices.size(), Ogre::MEMCATEGORY_GEOMETRY));

		for (int i = 0; i < Indices.size(); i++)
		{
			cubeIndices[i] = Indices[i];
		}

		Ogre::RenderSystem *renderSystem = Ogre::Root::getSingleton().getRenderSystem();
		Ogre::VaoManager *vaoManager = renderSystem->getVaoManager();

		try
		{
			indexBuffer = vaoManager->createIndexBuffer(Ogre::IndexBufferPacked::IT_16BIT,
				Indices.size(),
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

	std::pair<std::vector<LandscapeVisualization::OrientHolder>, std::vector<uint16_t>> LandscapeVisualization::processTriangles(GameSim::LandscapeMesh* Chunk)
	{
		std::vector<OrientHolder> Orientations;
		std::vector<uint16_t> NewIndices;
		Orientations.resize(Chunk->m_vertices.size());

		for (int trinagleNum = 0; trinagleNum < Chunk->m_triangles.size(); trinagleNum++)
		{
			auto& triangle = Chunk->m_triangles[trinagleNum];
			OrientHolder::Presentation TriangleState;

			if (triangle.normal.absolute().angle(btVector3(0, 1, 0)) < 0.9f)// ~51 degres
			{
				TriangleState = OrientHolder::UPDOWN;
			}
			else if (triangle.normal.absolute().angle(btVector3(1, 0, 0)) < 0.9f)// ~51 degres
			{
				TriangleState = OrientHolder::LEFTRIGHT;
			}
			else
			{
				TriangleState = OrientHolder::FRONTBACK;
			}
			
			for (int i = 0; i < 3; i++)
			{
				if (Orientations[triangle.indices[i]].currentPresentation == OrientHolder::UNDEF)
				{
					Orientations[triangle.indices[i]].OriginalVertex = triangle.indices[i];
					Orientations[triangle.indices[i]].accessPos(TriangleState) = triangle.indices[i];
					Orientations[triangle.indices[i]].currentPresentation = TriangleState;

					NewIndices.push_back(triangle.indices[i]);
				}
				else if (Orientations[triangle.indices[i]].accessPos(TriangleState) == std::numeric_limits<uint16_t>::max())
				{
					Orientations.emplace_back(triangle.indices[i], TriangleState);
					NewIndices.push_back(static_cast<uint16_t>(Orientations.size() - 1));
					Orientations[triangle.indices[i]].accessPos(TriangleState) = static_cast<uint16_t>(Orientations.size() - 1);
				}
				else
				{
					NewIndices.push_back(static_cast<uint16_t>(Orientations[triangle.indices[i]].accessPos(TriangleState)));
				}
			}
		}
		return std::make_pair(Orientations, NewIndices);
	}


	LandscapeVisualMesh LandscapeVisualization::DoCube()
	{
		LandscapeVisualMesh VisMesh;
		auto mSceneMgr = Ogre::Root::getSingleton().getSceneManager("ExampleSMInstance");
		VisMesh.marchingCubesItem = mSceneMgr->createItem("Cubexa.mesh");

		VisMesh.marchingCubeNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
		VisMesh.marchingCubeNode->attachObject(VisMesh.marchingCubesItem);
		VisMesh.marchingCubeNode->setPosition(0, 20.0f, 0);
		return VisMesh;
	}

	void LandscapeVisualMesh::Clear()
	{
		if (marchingCubesItem != nullptr)
		{
			marchingCubeNode->removeAndDestroyAllChildren();
			
			auto SceneManager = Ogre::Root::getSingleton().getSceneManager("ExampleSMInstance");
			SceneManager->destroyItem(marchingCubesItem);
			marchingCubesItem = nullptr;
		}
		if (mesh != nullptr)
		{
			Ogre::MeshManager::getSingleton().remove(mesh);
			mesh.setNull();
		}
	}
}