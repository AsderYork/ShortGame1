#include "stdafx.h"
#include "LandscapeTest.h"

#include "EngineController.h"
#include "SDL2_Service.h"
#include "Ogre_Service.h"
#include "CEGUI_Service.h"
#include "DebugLayout.h"
#include "DebugMapGraphicsService.h"
#include "ConsoleLayout.h"
#include "MapService.h"
#include "ScreenController.h"
#include "NetworkController.h"
#include "GameSimService.h"

#include "LoginScreen.h"
#include "MainGameScreen.h"
#include "DebugOverlayScreen.h"

#include <LandscapeMeshGenerator.h>
#include "LandscapeVisualization.h"

#include <LandscapeChunkPack.h>

#include <Caelum/Caelum.h>

#include <Ogre/Hlms/Unlit/OgreHlmsUnlit.h>
#include <OGRE\OgreMesh2.h>
#include <OGRE\OgreMeshManager2.h>
#include <OGRE\OgreSubMesh2.h>
#include <OGRE\OgreItem.h>

#include <Hasher.h>

namespace GEMTest {

	class ActualTester {
	public:
		GEM::Ogre_Service* m_OgreService;

		GEM::GameSim::LandscapeChunk m_ChunkToDraw[4];
		GEM::GameSim::LandscapeMeshGenerator m_MeshGenerator;
		GEM::GameSim::LandscapeMesh m_Mesh;
		GEM::LandscapeVisualMesh m_VisMesh;

		GEM::LandscapeVisualization m_LandscapeVisualization;

	public:


		void ChunkGen(int x, int z, GEM::GameSim::LandscapeChunk* target) {
			target->setVersion(1);
			target->setPosX(x);
			target->setPosZ(z);

			auto [ActualX, ActualZ] = target->getPosition();

			for (int x = 0; x < 16; x++)
			{
				for (int z = 0; z < 16; z++)
				{
					int ThisX = (ActualZ * 16) + x;
					int ThisZ = (ActualX * 16) + z;
					float NodeHeight = 20 + sin(ThisX / 3.0f) * 2 + sin(ThisZ / 3.83f + 1) * 1.43f;


					for (int y = 0; y < (int)floor(NodeHeight); y++)
					{
						target->getNode(z, y, x).SolidAmount = 255;
					}
					target->getNode(z, (int)floor(NodeHeight), x).SolidAmount = (int)(255 * (NodeHeight - floor(NodeHeight)));

					for (int y = 0; y < (int)floor(NodeHeight); y++)
					{
						target->getNode(z, y, x).SolidAmount = 255;
					}
					target->getNode(z, (int)floor(NodeHeight), x).SolidAmount = (int)(255 * (NodeHeight - floor(NodeHeight)));

					if (NodeHeight < 19) {
						target->getNode(z, (int)floor(NodeHeight), x).Solid = GEM::GameSim::LandscapeNode::SolidType::Sand;
					}
					else if (NodeHeight < 22) {
						target->getNode(z, (int)floor(NodeHeight), x).Solid = GEM::GameSim::LandscapeNode::SolidType::Soil_normal;
					}
					else {
						target->getNode(z, (int)floor(NodeHeight), x).Solid = GEM::GameSim::LandscapeNode::SolidType::Rock_Granite;
					}

				}
			}

		}

		bool IsTwoTrianglesHaveTheSameIndexes(GEM::GameSim::triangleType t1, GEM::GameSim::triangleType t2) {
			int hits = 0;
			for (auto t1index : t1.indices) {
				for (auto t2index : t2.indices) {
					if (t1index == t2index) {
						hits++;
						break;
					}
				}
			}

			return hits >= 3;

		}


		void InitializeChunks() {
			ChunkGen(0, 0, &m_ChunkToDraw[0]);
			ChunkGen(0, 1, &m_ChunkToDraw[1]);
			ChunkGen(1, 0, &m_ChunkToDraw[2]);
			ChunkGen(1, 1, &m_ChunkToDraw[3]);

			m_Mesh = m_MeshGenerator.Generate(&m_ChunkToDraw[0], &m_ChunkToDraw[1], &m_ChunkToDraw[2], &m_ChunkToDraw[3]);


			struct cntr {
				btVector3 pos;
				int count;
			};

			std::vector<std::pair<GEM::GameSim::triangleType, GEM::GameSim::triangleType>> sameTriangles;

			for (int i = 0; i < m_Mesh.m_triangles.size(); i++) {
				for (int j = 0; j < m_Mesh.m_triangles.size(); j++) {
					if (i > j && IsTwoTrianglesHaveTheSameIndexes(m_Mesh.m_triangles[i], m_Mesh.m_triangles[j])) {
						sameTriangles.push_back({ m_Mesh.m_triangles[i], m_Mesh.m_triangles[j] });
					}
				}
			}
		

			m_VisMesh = m_LandscapeVisualization.GenerateVisualMesh(&m_Mesh, &m_ChunkToDraw[0]);
			m_LandscapeVisualization.DoCube();

		}

		void Start() {
			auto SceneMgr = Ogre::Root::getSingleton().getSceneManager("ExampleSMInstance");
			SceneMgr->setAmbientLight(Ogre::ColourValue(0.8, 0.8, 0.8, 1), Ogre::ColourValue(0.8, 0.8, 0.8, 1), Ogre::Vector3(0, 1, 0));

			m_OgreService->AllowCameraMovement(true);

			InitializeChunks();
		}

	};


	LandscapeTest::LandscapeTest(GEM::Ogre_Service* OgreService) : m_OgreService(OgreService) {

		m_tester = new ActualTester;
		m_tester->m_OgreService = m_OgreService;
	}

	GEM::Service::ActionResult LandscapeTest::initialize()
	{
		m_tester->Start();

		return ActionResult::AR_OK;
	}

}