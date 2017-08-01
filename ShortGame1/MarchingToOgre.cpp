#include "stdafx.h"
#include "MarchingToOgre.h"
#include <OGRE\OgreItem.h>
#include <OGRE\OgreMaterialManager.h>
#include <OGRE\OgreMaterial.h>

#include <cmath>


#include <OGRE\OgreHlms.h>
#include <OGRE\OgreHlmsDatablock.h>
#include <OGRE\Hlms\Pbs\OgreHlmsPbsDatablock.h>
#include <OGRE\OgreHlmsManager.h>

#include <OGRE\OgreCamera.h>
#include <OGRE\OgreSceneQuery.h>

#include <OGRE\OgreMesh2.h>
#include <OGRE\OgreMeshManager2.h>
#include <OGRE\OgreSubMesh2.h>

struct MeshVertices
{
	float px, py, pz;   //Position
	float nx, ny, nz;   //Normals

	MeshVertices() {}
	MeshVertices(float _px, float _py, float _pz,
		float _nx, float _ny, float _nz) :
		px(_px), py(_py), pz(_pz),
		nx(_nx), ny(_ny), nz(_nz)	{}

	MeshVertices(float _px, float _py, float _pz) :
		px(_px), py(_py), pz(_pz),
		nx(0), ny(1), nz(0) {}
};


GEM::MarchingToOgre::MarchingToOgre(std::string MeshName, Ogre_Service* OgreService, MarchingCubesCalculator * Calc, int MapScale, int MeshScale)
	:m_meshName(MeshName), m_ogreService(OgreService), m_calc(Calc), m_mapScale(MapScale), m_meshScale(MeshScale)
{

}

void GEM::MarchingToOgre::updatePrepresentation()
{
	auto SceneMap = m_calc->getMap();
	int ObjNum = 0;
	for (int i = 0; i < SceneMap.size(); i++)
	{
		for (int j = 0; j < SceneMap[i].size(); j++)
		{
			for (int k = 0; k < SceneMap[i][j].size(); k++)
			{				
				ObjNum++;
			}
		}
	}
}

GEM::Service::ActionResult GEM::MarchingToOgre::initialize()
{
	m_calc->calculateMesh(0, 0, m_mapScale);
	auto SceneMap = m_calc->getMap();
	auto SceneManager = m_ogreService->getRoot()->getSceneManager("ExampleSMInstance");

	
	int id = 0;
	for (int i = 0; i < SceneMap.size(); i++)
	{
		for (int j = 0; j < SceneMap[i].size(); j++)
		{
			for (int k = 0; k < SceneMap[i][j].size(); k++)
			{
				Ogre::Item *tmpItem = SceneManager->createItem(m_meshName);
				Ogre::SceneNode *tmpSceneNode = SceneManager->getRootSceneNode(Ogre::SCENE_DYNAMIC)->createChildSceneNode(Ogre::SCENE_DYNAMIC);			
				CreateOrUpdateDatablock(id, m_calc->getValueOfNode(i,j,k) / 256.0);
				tmpItem->setDatablock("Test" + std::to_string(id));	

				tmpSceneNode->attachObject(tmpItem);
				tmpSceneNode->setScale(Ogre::Vector3(m_meshScale, m_meshScale, m_meshScale));
				tmpSceneNode->setPosition(i*m_mapScale, j*m_mapScale, k*m_mapScale);
				tmpSceneNode->setName("MarchigCubes/Node");

				m_itemsVector.push_back(tmpItem);
				m_nodesVector.push_back(tmpSceneNode);
				id++;
			}
		}
	}

	CreateSelectionDatablock();
	m_SelectionCountur = SceneManager->createItem(m_meshName);
	m_SelectionCountur->setDatablock("Picking");

	m_SelectionCounturNode = SceneManager->getRootSceneNode(Ogre::SCENE_DYNAMIC)->createChildSceneNode(Ogre::SCENE_DYNAMIC);
	m_SelectionCounturNode->setScale(Ogre::Vector3(m_meshScale*1.08, m_meshScale*1.08, m_meshScale*1.08));
	m_SelectionCounturNode->setPosition(0, 0,0);
	m_SelectionCounturNode->attachObject(m_SelectionCountur);
	m_SelectionCounturNode->setVisible(false);
	
	CreateMarchingCubeDatablock();
	CreateMesh();

	return ActionResult();
}

void GEM::MarchingToOgre::shutdown()
{
	for (auto& item : m_itemsVector)
	{
		item->detachFromParent();
	}
}

GEM::Service::ActionResult GEM::MarchingToOgre::preFrame(double timeDelta)
{
	int id = 0;
	auto SceneMap = m_calc->getMap();


	for (int i = 0; i < SceneMap.size(); i++)
	{
		for (int j = 0; j < SceneMap[i].size(); j++)
		{
			for (int k = 0; k < SceneMap[i][j].size(); k++)
			{
				CreateOrUpdateDatablock(id, m_calc->getValueOfNode(i, j, k) / 256.0);
				id++;
			}
		}
	}
	return ActionResult();
}

GEM::Service::ActionResult GEM::MarchingToOgre::frame(double timeDelta)
{
	return ActionResult();
}

GEM::Service::ActionResult GEM::MarchingToOgre::postFrame(double timeDelta)
{
	return ActionResult();
}

void GEM::MarchingToOgre::CreateOrUpdateDatablock(int id, float value)
{
	Ogre::HlmsManager *hlmsManager = m_ogreService->getRoot()->getHlmsManager();
	Ogre::HlmsTextureManager *hlmsTextureManager = hlmsManager->getTextureManager();

	auto hlmsPbs = hlmsManager->getHlms(Ogre::HLMS_PBS);

	Ogre::String datablockName = "Test" + Ogre::StringConverter::toString(id);

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

		datablock->setDiffuse(Ogre::Vector3(0.7f, 0.7f, 0.1f));
		Ogre::HlmsMacroblock macro = *(datablock->getMacroblock());
		macro.mCullMode = Ogre::CULL_NONE;
		datablock->setMacroblock(macro);

		datablock->setTransparency(0.8);

	}

	datablock->setDiffuse(Ogre::Vector3(0.0f, value, 0.0f));

}

void GEM::MarchingToOgre::CreateSelectionDatablock()
{
	Ogre::HlmsManager *hlmsManager = m_ogreService->getRoot()->getHlmsManager();
	Ogre::HlmsTextureManager *hlmsTextureManager = hlmsManager->getTextureManager();

	auto hlmsPbs = hlmsManager->getHlms(Ogre::HLMS_PBS);

	Ogre::String datablockName = "Picking";

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
	}

	datablock->setDiffuse(Ogre::Vector3(1.0f, 1.0f, 1.0f));
	datablock->setRoughness(0.0f);
	datablock->setBrdf(Ogre::PbsBrdf::BlinnPhongFullLegacy);

	Ogre::HlmsMacroblock macro = *(datablock->getMacroblock());
	macro.mCullMode = Ogre::CULL_ANTICLOCKWISE;
	datablock->setMacroblock(macro);

}

void GEM::MarchingToOgre::CreateMarchingCubeDatablock()
{
	Ogre::HlmsManager *hlmsManager = m_ogreService->getRoot()->getHlmsManager();
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

		datablock->setDiffuse(Ogre::Vector3(0.1f, 0.1f, 0.8f));
		Ogre::HlmsMacroblock macro = *(datablock->getMacroblock());
		macro.mCullMode = Ogre::CULL_NONE;
		datablock->setMacroblock(macro);

		datablock->setTransparency(0.5);

	}

	datablock->setDiffuse(Ogre::Vector3(0.3f, 0.3, 0.8f));
}

void GEM::MarchingToOgre::CreateMesh()
{
	Ogre::RenderSystem *renderSystem = m_ogreService->getRoot()->getRenderSystem();
	Ogre::VaoManager *vaoManager = renderSystem->getVaoManager();

	Ogre::MeshPtr mesh = Ogre::MeshManager::getSingleton().createManual("MarchingCubies", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	Ogre::SubMesh *subMesh = mesh->createSubMesh();

	//Vertex declaration
	Ogre::VertexElement2Vec vertexElements;
	vertexElements.push_back(Ogre::VertexElement2(Ogre::VET_FLOAT3, Ogre::VES_POSITION));
	vertexElements.push_back(Ogre::VertexElement2(Ogre::VET_FLOAT3, Ogre::VES_NORMAL));

	MeshVertices *meshVertices = reinterpret_cast<MeshVertices*>(OGRE_MALLOC_SIMD(sizeof(MeshVertices) * m_calc->getVertexes().size(), Ogre::MEMCATEGORY_GEOMETRY));

	for (int i = 0; i < m_calc->getVertexes().size(); i++)
	{
		meshVertices[i].px = (m_calc->getVertexes())[i].x;
		meshVertices[i].py = (m_calc->getVertexes())[i].y;
		meshVertices[i].pz = (m_calc->getVertexes())[i].z;

		meshVertices[i].nx = 0.0f;
		meshVertices[i].ny = 1.0f;
		meshVertices[i].nz = 0.0f;
	}

	Ogre::VertexBufferPacked *vertexBuffer = 0;
	try
	{
		//Create the actual vertex buffer.
		vertexBuffer = vaoManager->createVertexBuffer(vertexElements, m_calc->getVertexes().size(), Ogre::BT_IMMUTABLE, meshVertices, false);
	}
	catch (Ogre::Exception &e)
	{
		OGRE_FREE_SIMD(vertexBuffer, Ogre::MEMCATEGORY_GEOMETRY);
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

	//Set the bounds to get frustum culling and LOD to work correctly.
	float meshSize = m_calc->getMap().size()*m_mapScale;
	mesh->_setBounds(Ogre::Aabb(Ogre::Vector3::ZERO, Ogre::Vector3::UNIT_SCALE*meshSize), false);
	mesh->_setBoundingSphereRadius(meshSize);


	auto SceneManager = m_ogreService->getRoot()->getSceneManager("ExampleSMInstance");

	m_MarchingCubesItem = SceneManager->createItem("MarchingCubies");
	m_MarchingCubeNode = SceneManager->getRootSceneNode(Ogre::SCENE_DYNAMIC)->createChildSceneNode(Ogre::SCENE_DYNAMIC);

	m_MarchingCubeNode->attachObject(m_MarchingCubesItem);
	m_MarchingCubeNode->setPosition(0, 0, 0);

	m_MarchingCubesItem->setDatablock("MarchingCubesBlob");




}

Ogre::IndexBufferPacked * GEM::MarchingToOgre::createIndexBuffer()
{
	Ogre::IndexBufferPacked *indexBuffer = 0;

	Ogre::uint16 *cubeIndices = reinterpret_cast<Ogre::uint16*>(OGRE_MALLOC_SIMD(
		sizeof(Ogre::uint16) * m_calc->getIndexes().size(),
		Ogre::MEMCATEGORY_GEOMETRY));

	for (int i = 0; i < m_calc->getIndexes().size(); i++)
	{
		cubeIndices[i] = (m_calc->getIndexes())[i];
	}
	
	Ogre::RenderSystem *renderSystem = m_ogreService->getRoot()->getRenderSystem();
	Ogre::VaoManager *vaoManager = renderSystem->getVaoManager();

	try
	{
		indexBuffer = vaoManager->createIndexBuffer(Ogre::IndexBufferPacked::IT_16BIT,
			m_calc->getIndexes().size(),
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

void GEM::MarchingToOgre::mouseMoved(const SDL_Event & arg)
{
}

void GEM::MarchingToOgre::mousePressed(const SDL_MouseButtonEvent & arg)
{
	if (arg.button == SDL_BUTTON_LEFT)
	{
	 auto ray =	m_ogreService->getCamera()->getCameraToViewportRay(0.5, 0.5);
	 Ogre::RaySceneQuery *Querry = m_ogreService->getRoot()->getSceneManager("ExampleSMInstance")->createRayQuery(ray);

	 m_SelectionCounturNode->setVisible(false);
	 auto Result = Querry->execute();
	 if (Result.size() > 0)
	 {
		 for (auto& ResultObj : Result)
		 {
			 if (ResultObj.movable->getParentNode()->getName() == "MarchigCubes/Node")
			 {
				 m_SelectionCounturNode->setPosition(ResultObj.movable->getParentNode()->getPosition());
				 m_SelectionCounturNode->setVisible(true);
				 break;
			 }
		 }
	 }

	 m_ogreService->getRoot()->getSceneManager("ExampleSMInstance")->destroyQuery(Querry);
	}

	if (arg.button == SDL_BUTTON_RIGHT)
	{
		if (m_SelectionCountur->isVisible())//If something is chosen
		{
			int x, y, z;//Position of node in a map
			
			auto SceneNodePose = m_SelectionCounturNode->getPosition();
			x = std::round(SceneNodePose.x / m_mapScale);
			y = std::round(SceneNodePose.y / m_mapScale);
			z = std::round(SceneNodePose.z / m_mapScale);

			if (m_calc->getValueOfNode(x, y, z) != 0)
			{
				m_calc->setValueOfNode(x, y, z, 0);
			}
			else
			{
				m_calc->setValueOfNode(x, y, z, 255);
			}
		}
	}
}

void GEM::MarchingToOgre::mouseReleased(const SDL_MouseButtonEvent & arg)
{
}
