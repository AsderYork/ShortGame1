#include "stdafx.h"
#include "LandscapeVisualization.h"
#include "LandscapeChunk.h"

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
	class ShaderCasher {

	private:

		struct Shader {
			
			static int _Id;

			Ogre::Vector3 Diffuse;
			Ogre::Vector3 Specular;
			float Roughness;
			float Transparency;
			bool NoCull;
			int Id;

			Ogre::HlmsPbsDatablock* pbsDatablock;

			bool IsOfRightParams(Ogre::Vector3 diffuse, Ogre::Vector3 specular, float roughless, float transperency, bool noCull) {
				return diffuse == Diffuse && specular == Specular && Roughness == roughless && Transparency == transperency && NoCull == noCull;
			}

			Shader(Ogre::Vector3 diffuse, Ogre::Vector3 specular, float roughless, float transperency, bool noCull) {
				Diffuse = diffuse;
				Specular = specular;
				Roughness = roughless;
				Transparency = transperency;
				NoCull = noCull;
				Id = _Id++;

				pbsDatablock = CreateShader(Diffuse, Specular, Roughness, Transparency, NoCull, Id);
			}

			Ogre::HlmsPbsDatablock* CreateShader(Ogre::Vector3 diffuse, Ogre::Vector3 specular, float roughness, float transparency, bool NoCull, int id) {

				Ogre::HlmsManager* hlmsManager = Ogre::Root::getSingleton().getHlmsManager();
				Ogre::HlmsTextureManager* hlmsTextureManager = hlmsManager->getTextureManager();

				auto hlmsPbs = hlmsManager->getHlms(Ogre::HLMS_PBS);

				Ogre::String datablockName = "DMGS_VisionMat" + id;

				Ogre::HlmsPbsDatablock* datablock;
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

				datablock->setDiffuse(diffuse);
				datablock->setSpecular(specular);
				datablock->setRoughness(roughness);
				datablock->setBrdf(Ogre::PbsBrdf::BlinnPhongFullLegacy);
				datablock->setTransparency(transparency);

				Ogre::HlmsMacroblock macro = *(datablock->getMacroblock());
				macro.mCullMode = NoCull ? Ogre::CULL_NONE : Ogre::CULL_CLOCKWISE;
				macro.mPolygonMode = Ogre::PM_WIREFRAME;
				datablock->setMacroblock(macro);

				return datablock;
			}

		};

		static std::vector<Shader> shaders;

	public:

		static Ogre::HlmsPbsDatablock* GetShader(Ogre::Vector3 diffuse = Ogre::Vector3(1.0f, 1.0f, 1.0f), float transparency = 1.0f, Ogre::Vector3 specular = Ogre::Vector3(1.0f, 1.0f, 1.0f), float roughness = 1.0f, bool noCull = false) {

			auto shader = std::find_if(shaders.begin(), shaders.end(), [&](Shader x) {return x.IsOfRightParams(diffuse, specular, roughness, transparency, noCull); });

			if (shader == shaders.end()) {
				return shaders.emplace_back(diffuse, specular, roughness, transparency, noCull).pbsDatablock;
			}
			else {
				return shader->pbsDatablock;
			}


		}

	};

	int ShaderCasher::Shader::_Id = 0;
	std::vector<ShaderCasher::Shader> ShaderCasher::shaders;


	struct MeshVertices
	{
		float px = 0, py = 0, pz = 0;   //Position
		float nx = 0, ny = 1, nz = 0;   //Normals
		float nu = 0, nv = 0; //Texture Coordinates 1
		//float val = 0.0f;
		uint32_t LandType1 = 0;
		uint32_t LandType2 = 0;

	};

	LandscapeVisualMesh LandscapeVisualization::GenerateVisualMesh(GameSim::LandscapeMesh* Chunk, GameSim::LandscapeChunk* ChunkData)
	{
		
		auto[ChunkPosX, ChunkPosZ] = ChunkData->getPosition();
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
		vertexElements.push_back(Ogre::VertexElement2(Ogre::VET_UINT2, Ogre::VES_TEXTURE_COORDINATES));
		
		auto[Orientations, NewIndices] = processTriangles(Chunk);

		MeshVertices *meshVertices = reinterpret_cast<MeshVertices*>(OGRE_MALLOC_SIMD(sizeof(MeshVertices) * Orientations.size(), Ogre::MEMCATEGORY_GEOMETRY));

		
		//Translate VertexList from Generator to Ogre
		for (int i = 0; i < Orientations.size(); i++)
		{
			auto& Vertex = Chunk->m_vertices[Orientations[i].OriginalVertex];

			//if (Vertex.normal.fuzzyZero()) { continue; }

			meshVertices[i].px = static_cast<float>(Vertex.pos.x());
			meshVertices[i].py = static_cast<float>(Vertex.pos.y());
			meshVertices[i].pz = static_cast<float>(Vertex.pos.z());

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

			float BlendWeight = Vertex.pos.x() + Vertex.pos.y() + Vertex.pos.z();
			BlendWeight = BlendWeight - floorf(BlendWeight);

			/*GameSim::LandscapeNode* DominantNode = &ChunkData->getNode(static_cast<uint32_t>(floor(Vertex.pos.x())),
				static_cast<uint32_t>(floor(Vertex.pos.y())),
					static_cast<uint32_t>(floor(Vertex.pos.z())));*/

			GameSim::LandscapeNode* DominantNode = Vertex.OriginalNode1;
			//GameSim::LandscapeNode* SubdominantNode = BlendWeight > 0.5f ? Vertex.OriginalNode1 : Vertex.OriginalNode2;

			meshVertices[i].LandType1 = static_cast<uint32_t>(DominantNode->Solid);// +static_cast<uint32_t>(SubdominantNode->Solid) * 0x00000;
			meshVertices[i].LandType2 = *reinterpret_cast<uint32_t*>(&BlendWeight);


			/*switch (DominantNode->Solid)
			{
			case GameSim::LandscapeNode::SolidType::Sand:
				meshVertices[i].LandType1 = 0x00000001;
				//meshVertices[i].val = 0.0f;

				break;
			case GameSim::LandscapeNode::SolidType::Soil_normal:


				meshVertices[i].LandType1 = 0x00000002;
				//meshVertices[i].val = 1.0f;
				break;
			default:

				meshVertices[i].LandType1 = 0x00000003;
				//meshVertices[i].val = 0.5f;
				break;
			}*/
								

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
			Ogre::Vector3(meshSize, GameSim::LandscapeChunk_Height / 2, meshSize),
			Ogre::Vector3(meshSize, GameSim::LandscapeChunk_Height / 2, meshSize)),
			false);
		ReturnMesh.mesh->_setBoundingSphereRadius(meshRadius);

		subMesh->setMaterialName("Landscape1");


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


	static void CreateCubeMesh() {

		Ogre::RenderSystem* renderSystem = Ogre::Root::getSingleton().getRenderSystem();
		Ogre::VaoManager* vaoManager = renderSystem->getVaoManager();
		auto SceneManager = Ogre::Root::getSingleton().getSceneManager("ExampleSMInstance");

		std::string MeshName = "Cube";

		auto mesh = Ogre::MeshManager::getSingleton().createManual(MeshName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		Ogre::SubMesh *subMesh = mesh->createSubMesh();

		Ogre::VertexElement2Vec vertexElements;
		vertexElements.push_back(Ogre::VertexElement2(Ogre::VET_FLOAT3, Ogre::VES_POSITION));
		vertexElements.push_back(Ogre::VertexElement2(Ogre::VET_FLOAT3, Ogre::VES_NORMAL));
		vertexElements.push_back(Ogre::VertexElement2(Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES));
		vertexElements.push_back(Ogre::VertexElement2(Ogre::VET_UINT2, Ogre::VES_TEXTURE_COORDINATES));

		MeshVertices* meshVertices = reinterpret_cast<MeshVertices*>(OGRE_MALLOC_SIMD(sizeof(MeshVertices) * 8, Ogre::MEMCATEGORY_GEOMETRY));

		std::vector<std::tuple<int, int, int>> vertices = {
			{-1.0, -1.0,  1.0},
			{ 1.0, -1.0,  1.0},
			{ 1.0,  1.0,  1.0},
			{-1.0,  1.0,  1.0},
			// back
			{-1.0, -1.0, -1.0},
			{ 1.0, -1.0, -1.0},
			{ 1.0,  1.0, -1.0},
			{-1.0,  1.0, -1.0}
		};

		int i = 0;
		for (auto& x : vertices) {
			meshVertices[i].px = static_cast<float>(std::get<0>(x));
			meshVertices[i].py = static_cast<float>(std::get<1>(x));
			meshVertices[i].pz = static_cast<float>(std::get<2>(x));
													
			meshVertices[i].nx = static_cast<float>(std::get<0>(x));
			meshVertices[i].ny = static_cast<float>(std::get<1>(x));
			meshVertices[i].nz = static_cast<float>(std::get<2>(x));
			i++;
		}
		

		Ogre::VertexBufferPacked* vertexBuffer = 0;
		try
		{
			//Create the actual vertex buffer.
			vertexBuffer = vaoManager->createVertexBuffer(vertexElements, 8, Ogre::BT_IMMUTABLE, meshVertices, false);
		}
		catch (Ogre::Exception & e)
		{
			OGRE_FREE_SIMD(vertexBuffer, Ogre::MEMCATEGORY_GEOMETRY);
			vertexBuffer = 0;
			throw e;
		}

		Ogre::VertexBufferPackedVec vertexBuffers;
		vertexBuffers.push_back(vertexBuffer);

		Ogre::IndexBufferPacked* indexBuffer = 0;

		Ogre::uint16* cubeIndices = reinterpret_cast<Ogre::uint16*>(OGRE_MALLOC_SIMD(sizeof(Ogre::uint16) * 36, Ogre::MEMCATEGORY_GEOMETRY));

		Ogre::uint16 Indices[] = {
			// front
			0, 1, 2,
			2, 3, 0,
			// right
			1, 5, 6,
			6, 2, 1,
			// back
			7, 6, 5,
			5, 4, 7,
			// left
			4, 0, 3,
			3, 7, 4,
			// bottom
			4, 5, 1,
			1, 0, 4,
			// top
			3, 2, 6,
			6, 7, 3
		};
		
		for (int i = 0; i < sizeof(Indices) / sizeof(Ogre::uint16); i++) {
			cubeIndices[i] = Indices[i];
		}


		
		try
		{
			indexBuffer = vaoManager->createIndexBuffer(Ogre::IndexBufferPacked::IT_16BIT,
				36,
				Ogre::BT_IMMUTABLE,
				cubeIndices, true);
		}
		catch (Ogre::Exception & e)
		{
			// When keepAsShadow = true, the memory will be freed when the index buffer is destroyed.
			// However if for some weird reason there is an exception raised, the memory will
			// not be freed, so it is up to us to do so.
			// The reasons for exceptions are very rare. But we're doing this for correctness.
			OGRE_FREE_SIMD(indexBuffer, Ogre::MEMCATEGORY_GEOMETRY);
			indexBuffer = 0;
			throw e;
		}
		
		Ogre::VertexArrayObject* vao = vaoManager->createVertexArrayObject(
			vertexBuffers, indexBuffer, Ogre::OT_TRIANGLE_LIST);

		subMesh->mVao[Ogre::VpNormal].push_back(vao);
		//Use the same geometry for shadow casting.
		subMesh->mVao[Ogre::VpShadow].push_back(vao);

		float meshSize = 2.0f;
		float meshRadius = 1.732051f;
		mesh->_setBounds(Ogre::Aabb(
			Ogre::Vector3(0.0f, 0.0f, 0.0f),
			Ogre::Vector3(1.0f, 1.0f, 1.0f)));
		mesh->_setBoundingSphereRadius(meshRadius);
					   		 
	}


	LandscapeVisualMesh LandscapeVisualization::DoCube(float x, float y, float z, Ogre::Vector3 colour, float transperency, Ogre::Vector3 scale)
	{

		auto mesh = Ogre::MeshManager::getSingleton().getByName("Cube");
		if (mesh.isNull()) {
			CreateCubeMesh();
		}
		mesh = Ogre::MeshManager::getSingleton().getByName("Cube");
	   
		LandscapeVisualMesh VisMesh;
		auto mSceneMgr = Ogre::Root::getSingleton().getSceneManager("ExampleSMInstance");
		VisMesh.marchingCubesItem = mSceneMgr->createItem("Cube");

		VisMesh.marchingCubesItem->setDatablock(ShaderCasher::GetShader(colour, transperency, Ogre::Vector3(1.0f), 1.0f, true));

		VisMesh.marchingCubeNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
		VisMesh.marchingCubeNode->attachObject(VisMesh.marchingCubesItem);


		VisMesh.marchingCubeNode->setPosition(x, y, z);
		VisMesh.marchingCubeNode->setScale(scale);
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

	LandscapeVisualMesh LandscapeVisualization::GenerateMeshFromVertices(std::vector<Ogre::Vector3> vertices, std::vector<UINT> indices, Ogre::Vector3 originPos, int size) {

		if (vertices.size() == 0 || indices.size() == 0) {
			return LandscapeVisualMesh();
		}

		Ogre::RenderSystem* renderSystem = Ogre::Root::getSingleton().getRenderSystem();
		Ogre::VaoManager* vaoManager = renderSystem->getVaoManager();
		auto SceneManager = Ogre::Root::getSingleton().getSceneManager("ExampleSMInstance");

		static int meshCounter = 0;

		std::string MeshName = "MarchingCubes" + std::to_string(meshCounter++);

		auto mesh = Ogre::MeshManager::getSingleton().createManual(MeshName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		Ogre::SubMesh* subMesh = mesh->createSubMesh();

		Ogre::VertexElement2Vec vertexElements;
		vertexElements.push_back(Ogre::VertexElement2(Ogre::VET_FLOAT3, Ogre::VES_POSITION));
		vertexElements.push_back(Ogre::VertexElement2(Ogre::VET_FLOAT3, Ogre::VES_NORMAL));
		vertexElements.push_back(Ogre::VertexElement2(Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES));
		vertexElements.push_back(Ogre::VertexElement2(Ogre::VET_UINT2, Ogre::VES_TEXTURE_COORDINATES));

		MeshVertices* meshVertices = reinterpret_cast<MeshVertices*>(OGRE_MALLOC_SIMD(sizeof(MeshVertices) * vertices.size(), Ogre::MEMCATEGORY_GEOMETRY));

		
		int i = 0;
		for (auto& vertex: vertices) {
			meshVertices[i].px = static_cast<float>(vertex.x);
			meshVertices[i].py = static_cast<float>(vertex.y);
			meshVertices[i].pz = static_cast<float>(vertex.z);

			i++;
		}


		Ogre::VertexBufferPacked* vertexBuffer = 0;
		try
		{
			//Create the actual vertex buffer.
			vertexBuffer = vaoManager->createVertexBuffer(vertexElements, vertices.size(), Ogre::BT_IMMUTABLE, meshVertices, false);
		}
		catch (Ogre::Exception & e)
		{
			OGRE_FREE_SIMD(vertexBuffer, Ogre::MEMCATEGORY_GEOMETRY);
			vertexBuffer = 0;
			throw e;
		}

		Ogre::VertexBufferPackedVec vertexBuffers;
		vertexBuffers.push_back(vertexBuffer);

		Ogre::IndexBufferPacked* indexBuffer = 0;

		Ogre::uint16* cubeIndices = reinterpret_cast<Ogre::uint16*>(OGRE_MALLOC_SIMD(sizeof(Ogre::uint16) * indices.size(), Ogre::MEMCATEGORY_GEOMETRY));

		for (int i = 0; i < indices.size(); i++)
			cubeIndices[i] = indices[i];

		try
		{
			indexBuffer = vaoManager->createIndexBuffer(Ogre::IndexBufferPacked::IT_16BIT, indices.size(), Ogre::BT_IMMUTABLE,	cubeIndices, true);
		}
		catch (Ogre::Exception & e)
		{
			// When keepAsShadow = true, the memory will be freed when the index buffer is destroyed.
			// However if for some weird reason there is an exception raised, the memory will
			// not be freed, so it is up to us to do so.
			// The reasons for exceptions are very rare. But we're doing this for correctness.
			OGRE_FREE_SIMD(indexBuffer, Ogre::MEMCATEGORY_GEOMETRY);
			indexBuffer = 0;
			throw e;
		}

		Ogre::VertexArrayObject* vao = vaoManager->createVertexArrayObject(
			vertexBuffers, indexBuffer, Ogre::OT_TRIANGLE_LIST);

		subMesh->mVao[Ogre::VpNormal].push_back(vao);
		//Use the same geometry for shadow casting.
		subMesh->mVao[Ogre::VpShadow].push_back(vao);

		float meshRadius = sqrt(3 * size* size);
		mesh->_setBounds(Ogre::Aabb(
			Ogre::Vector3(0.0f, 0.0f, 0.0f),
			Ogre::Vector3(size)));
		mesh->_setBoundingSphereRadius(meshRadius);


		LandscapeVisualMesh ReturnMesh;

		ReturnMesh.marchingCubesItem = SceneManager->createItem(MeshName);

		ReturnMesh.marchingCubesItem->setDatablock(ShaderCasher::GetShader(Ogre::Vector3(0.05f, 0.4f, 0.2f), 1.0f));

		ReturnMesh.marchingCubeNode = SceneManager->getRootSceneNode(Ogre::SCENE_DYNAMIC)->createChildSceneNode(Ogre::SCENE_DYNAMIC);

		ReturnMesh.marchingCubeNode->attachObject(ReturnMesh.marchingCubesItem);
		ReturnMesh.marchingCubeNode->setPosition(originPos);
		return ReturnMesh;


		

	}
}