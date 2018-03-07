#include "stdafx.h"
#include "Ogre_Service.h"
#include <OGRE\OgreRenderWindow.h>
#include <OGRE\OgreCamera.h>
#include <OGRE\Compositor\OgreCompositorManager2.h>
#include <OGRE\OgreMeshManager2.h>
#include <OGRE\OgreMesh2.h>
#include <OGRE\OgreSubMesh2.h>
#include <OGRE\OgreItem.h>
#include <OGRE\OgreConfigFile.h>
#include <OGRE\OgreArchiveManager.h>
#include <OGRE\Hlms\Unlit\OgreHlmsUnlit.h>
#include <OGRE\Hlms\Pbs\OgreHlmsPbs.h>
#include <OGRE\OgreHlmsManager.h>


namespace GEM
{
	struct CubeVertices
	{
		float px, py, pz;   //Position
		float nx, ny, nz;   //Normals

		CubeVertices() {}
		CubeVertices(float _px, float _py, float _pz,
			float _nx, float _ny, float _nz) :
			px(_px), py(_py), pz(_pz),
			nx(_nx), ny(_ny), nz(_nz)
		{
		}
	};

	GEM::Ogre_Service::ActionResult GEM::Ogre_Service::initialize()
	{
		m_ogreLog.connectToOgreLog();

		m_root = std::make_unique<Ogre::Root>("", "ogre.cfg");

#ifdef _DEBUG
		m_root->loadPlugin("RenderSystem_Direct3D11_d");
		m_root->loadPlugin("RenderSystem_GL3Plus_d");
		m_root->loadPlugin("Plugin_ParticleFX_d");
#else
		m_root->loadPlugin("RenderSystem_Direct3D11");
		m_root->loadPlugin("RenderSystem_GL3Plus");
		m_root->loadPlugin("Plugin_ParticleFX");
#endif

		if ( (!m_root->restoreConfig()) || (false))
		{
			if (!m_root->showConfigDialog())
			{
				LOGCATEGORY("Ogre_Service/initialize").error("Can't restore and can't make new config!");
				return ActionResult::AR_ERROR;
			}
		}

		m_root->initialise(false);
		Ogre::ConfigOptionMap& cfgOpts = m_root->getRenderSystem()->getConfigOptions();

		auto WidthHeight = getWidthHeighgtFromConfig();
		auto fullscreen = cfgOpts["Full Screen"].currentValue;

		auto Hwnd = m_sdlController->MakeWindow(WidthHeight.first, WidthHeight.second, (fullscreen == "Yes" ? true : false ));

		Ogre::NameValuePairList params;

		params.insert(std::make_pair("externalWindowHandle", Hwnd));
		params.insert(std::make_pair("title", "A window"));
		params.insert(std::make_pair("gamma", "true"));
		params.insert(std::make_pair("FSAA", cfgOpts["FSAA"].currentValue));
		params.insert(std::make_pair("vsync", cfgOpts["VSync"].currentValue));

		m_renderWindow = Ogre::Root::getSingleton().createRenderWindow("A window", WidthHeight.first, WidthHeight.second,
			(fullscreen == "Yes" ? true : false),
			&params);


		setupResources();
		RegisterHLMS();

		Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups(true);
		chooseSceneManager();
		createCamera();
		m_workspace = createWorkspace();
		//CreateCube();

		Ogre::Light *light = m_sceneManager->createLight();
		Ogre::SceneNode *lightNode = m_sceneManager->getRootSceneNode()->createChildSceneNode();
		lightNode->attachObject(light);
		light->setPowerScale(Ogre::Math::PI); //Since we don't do HDR, counter the PBS' division by PI
		light->setType(Ogre::Light::LT_DIRECTIONAL);
		light->setDirection(Ogre::Vector3(-1, -1, -1).normalisedCopy());

		return ActionResult::AR_OK;
	}

	void GEM::Ogre_Service::shutdown()
	{
		m_root.release();
	}

	GEM::Ogre_Service::ActionResult GEM::Ogre_Service::preFrame(float timeDelta)
	{
		tmpCamera.AjustPosition(timeDelta);
		return ActionResult();
	}

	GEM::Ogre_Service::ActionResult GEM::Ogre_Service::frame(float timeDelta)
	{
		m_root->renderOneFrame(timeDelta);
		return ActionResult();
	}

	GEM::Ogre_Service::ActionResult GEM::Ogre_Service::postFrame(float timeDelta)
	{
		return ActionResult();
	}
	Ogre_Service::Ogre_Service(SDL_Controller * SDL) : m_sdlController(SDL)
	{
	}
	Ogre::Root* Ogre_Service::getRoot()
	{
		return m_root.get();
	}
	Ogre::Camera * Ogre_Service::getCamera()
	{
		return tmpCamera.getCamera();
	}
	void Ogre_Service::AllowCameraMovement(bool State)
	{
		tmpCamera.ShouldReciveInput(State);
	}
	bool Ogre_Service::isCameraAllowedToMove()
	{
		return tmpCamera.isRecivingInput();
	}
	std::pair<int, int> Ogre_Service::getWidthHeighgtFromConfig()
	{
		Ogre::ConfigOptionMap& cfgOpts = m_root->getRenderSystem()->getConfigOptions(); 
		auto opt = cfgOpts.find("Video Mode");
		int width = 1280;
		int height = 720;
		if (opt != cfgOpts.end())
		{
			//Ignore leading space
			const Ogre::String::size_type start = opt->second.currentValue.find_first_of("012356789");
			//Get the width and height
			Ogre::String::size_type widthEnd = opt->second.currentValue.find(' ', start);
			// we know that the height starts 3 characters after the width and goes until the next space
			Ogre::String::size_type heightEnd = opt->second.currentValue.find(' ', widthEnd + 3);
			// Now we can parse out the values
			width = Ogre::StringConverter::parseInt(opt->second.currentValue.substr(0, widthEnd));
			height = Ogre::StringConverter::parseInt(opt->second.currentValue.substr(
				widthEnd + 3, heightEnd));
		}

		return std::make_pair(width, height);
	}
	void Ogre_Service::chooseSceneManager()
	{
		Ogre::InstancingThreadedCullingMethod threadedCullingMethod =
			Ogre::INSTANCING_CULLING_SINGLETHREAD;

		const size_t numThreads = 1;

		// Create the SceneManager, in this case a generic one
		m_sceneManager = m_root->createSceneManager(Ogre::ST_GENERIC,
			numThreads,
			threadedCullingMethod,
			"ExampleSMInstance");


		//Set sane defaults for proper shadow mapping
		m_sceneManager->setShadowDirectionalLightExtrusionDistance(500.0f);
		m_sceneManager->setShadowFarDistance(500.0f);
		//m_sceneManager->setAmbientLight(Ogre::ColourValue(0.8, 0.8, 0.8, 1), Ogre::ColourValue(0.8, 0.8, 0.8, 1), Ogre::Vector3(0, 1, 0));

		m_sceneManager->setAmbientLight(Ogre::ColourValue(1 ,1 ,1, 1), Ogre::ColourValue(1, 1, 1, 1), Ogre::Vector3(0, 1, 0));
	}
	void Ogre_Service::createCamera()
	{
		tmpCamera.SetCamera(m_sceneManager, m_sdlController);
		
	}
	Ogre::CompositorWorkspace * Ogre_Service::createWorkspace()
	{
		auto compositorManager = m_root->getCompositorManager2();

		const Ogre::String workspaceName("Demo Workspace");
		if (!compositorManager->hasWorkspaceDefinition(workspaceName))
		{
			compositorManager->createBasicWorkspaceDef(workspaceName, Ogre::ColourValue(0.2f, 0.4f, 0.6f), Ogre::IdString());
		}

		return compositorManager->addWorkspace(m_sceneManager, m_renderWindow, tmpCamera.getCamera(), workspaceName, true);
	}
	/*
	void Ogre_Service::CreateCube()
	{
		Ogre::RenderSystem *renderSystem = m_root->getRenderSystem();
		Ogre::VaoManager *vaoManager = renderSystem->getVaoManager();

		Ogre::MeshPtr mesh = Ogre::MeshManager::getSingleton().createManual("ACube", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		Ogre::SubMesh *subMesh = mesh->createSubMesh();

		//Vertex declaration
		Ogre::VertexElement2Vec vertexElements;
		vertexElements.push_back(Ogre::VertexElement2(Ogre::VET_FLOAT3, Ogre::VES_POSITION));
		vertexElements.push_back(Ogre::VertexElement2(Ogre::VET_FLOAT3, Ogre::VES_NORMAL));

		CubeVertices *cubeVertices = reinterpret_cast<CubeVertices*>(OGRE_MALLOC_SIMD(
			sizeof(CubeVertices) * 8,
			Ogre::MEMCATEGORY_GEOMETRY));

		const CubeVertices originalVertices[8] =
		{
			CubeVertices(-1.0f, -1,  1, -0.57737, -0.57737,  0.57737),
			CubeVertices(1, -1,  1,  0.57737, -0.57737,  0.57737),
			CubeVertices(1,  1,  1,  0.57737,  0.57737,  0.57737),
			CubeVertices(-1,  1,  1, -0.57737,  0.57737,  0.57737),
			CubeVertices(-1, -1, -1, -0.57737, -0.57737, -0.57737),
			CubeVertices(1, -1, -1,  0.57737, -0.57737, -0.57737),
			CubeVertices(1,  1, -1,  0.57737,  0.57737, -0.57737),
			CubeVertices(-1,  1, -1, -0.57737,  0.57737, -0.57737)
		};
		memcpy(cubeVertices, originalVertices, sizeof(CubeVertices) * 8);

		Ogre::VertexBufferPacked *vertexBuffer = 0;
		try
		{
			//Create the actual vertex buffer.
			vertexBuffer = vaoManager->createVertexBuffer(vertexElements, 8,Ogre::BT_IMMUTABLE,	cubeVertices, false);
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
		mesh->_setBounds(Ogre::Aabb(Ogre::Vector3::ZERO, Ogre::Vector3::UNIT_SCALE), false);
		mesh->_setBoundingSphereRadius(1.732f);


	}
	void Ogre_Service::CreateCubeFromMesh()
	{

		Ogre::Item *item = m_sceneManager->createItem("Cube_d.mesh",
			Ogre::ResourceGroupManager::
			AUTODETECT_RESOURCE_GROUP_NAME,
			Ogre::SCENE_DYNAMIC);

		m_node = m_sceneManager->getRootSceneNode(Ogre::SCENE_DYNAMIC)->
			createChildSceneNode(Ogre::SCENE_DYNAMIC);

		m_node->attachObject(item);
	}
	*/
	Ogre::IndexBufferPacked * Ogre_Service::createIndexBuffer()
	{
		Ogre::IndexBufferPacked *indexBuffer = 0;

		const Ogre::uint16 c_indexData[3 * 2 * 6] =
		{
			0, 1, 2, 2, 3, 0, //Front face
			6, 5, 4, 4, 7, 6, //Back face

			3, 2, 6, 6, 7, 3, //Top face
			5, 1, 0, 0, 4, 5, //Bottom face

			4, 0, 3, 3, 7, 4, //Left face
			6, 2, 1, 1, 5, 6, //Right face
		};

		Ogre::uint16 *cubeIndices = reinterpret_cast<Ogre::uint16*>(OGRE_MALLOC_SIMD(
			sizeof(Ogre::uint16) * 3 * 2 * 6,
			Ogre::MEMCATEGORY_GEOMETRY));
		memcpy(cubeIndices, c_indexData, sizeof(c_indexData));

		Ogre::RenderSystem *renderSystem = m_root->getRenderSystem();
		Ogre::VaoManager *vaoManager = renderSystem->getVaoManager();

		try
		{
			indexBuffer = vaoManager->createIndexBuffer(Ogre::IndexBufferPacked::IT_16BIT,
				3 * 2 * 6,
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

	void Ogre_Service::RegisterHLMS()
	{
		Ogre::ConfigFile cf;
		cf.load("../Resources/resources.cfg");


		Ogre::String dataFolder = cf.getSetting("DoNotUseAsResource", "Hlms", "");


		if (dataFolder.empty())
			dataFolder = "./";
		else if (*(dataFolder.end() - 1) != '/')
			dataFolder += "/";

		Ogre::RenderSystem *renderSystem = m_root->getRenderSystem();

		Ogre::String shaderSyntax = "GLSL";
		if (renderSystem->getName() == "Direct3D11 Rendering Subsystem")
			shaderSyntax = "HLSL";
		else if (renderSystem->getName() == "Metal Rendering Subsystem")
			shaderSyntax = "Metal";
		Ogre::Archive *archiveLibrary = Ogre::ArchiveManager::getSingletonPtr()->load(
			dataFolder + "Hlms/Common/" + shaderSyntax,
			"FileSystem", true);

		Ogre::Archive *archivePbsLibraryAny = Ogre::ArchiveManager::getSingletonPtr()->load(
			dataFolder + "Hlms/Pbs/Any",
			"FileSystem", true);

		Ogre::ArchiveVec library;
		library.push_back(archiveLibrary);

		Ogre::Archive *archivePbs = Ogre::ArchiveManager::getSingletonPtr()->load(
			dataFolder + "Hlms/Pbs/" + shaderSyntax,
			"FileSystem", true);

		library.push_back(archivePbsLibraryAny);
		Ogre::HlmsPbs *hlmsPbs = OGRE_NEW Ogre::HlmsPbs(archivePbs, &library);
		Ogre::Root::getSingleton().getHlmsManager()->registerHlms(hlmsPbs);
		library.pop_back();

		if (renderSystem->getName() == "Direct3D11 Rendering Subsystem")
		{
			//Set lower limits 512kb instead of the default 4MB per Hlms in D3D 11.0
			//and below to avoid saturating AMD's discard limit (8MB) or
			//saturate the PCIE bus in some low end machines.
			bool supportsNoOverwriteOnTextureBuffers;
			renderSystem->getCustomAttribute("MapNoOverwriteOnDynamicBufferSRV",
				&supportsNoOverwriteOnTextureBuffers);

			if (!supportsNoOverwriteOnTextureBuffers)
			{
				hlmsPbs->setTextureBufferDefaultSize(512 * 1024);
			}
		}
	}

	void Ogre_Service::setupResources()
	{ // Load resource paths from config file
		Ogre::ConfigFile cf;
		cf.load("../Resources/resources.cfg");

		// Go through all sections & settings in the file
		Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

		Ogre::String secName, typeName, archName;
		while (seci.hasMoreElements())
		{
			secName = seci.peekNextKey();
			Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();

			if (secName != "Hlms")
			{
				Ogre::ConfigFile::SettingsMultiMap::iterator i;
				for (i = settings->begin(); i != settings->end(); ++i)
				{
					typeName = i->first;
					archName = i->second;
					Ogre::ResourceGroupManager::getSingleton().addResourceLocation(archName, typeName, secName);
				}
			}
		}
	}
	
	//TEMPORAL CAMERA.
	void MovableCamera::SetCamera(Ogre::SceneManager * sceneManager, SDL_Controller * SDLController)
	{
		m_camera = sceneManager->createCamera("Main Camera");

		m_camera->setPosition(Ogre::Vector3(0, 0, 15));
		// Look back along -Z
		m_camera->lookAt(Ogre::Vector3(0, 0, 0));
		m_camera->setNearClipDistance(0.2f);
		m_camera->setFarClipDistance(1000.0f);
		m_camera->setAutoAspectRatio(true);

		SDLController->registerKeyboardListener(this);
		SDLController->registerMouseListener(this);
	}
	void MovableCamera::AjustPosition(float timeDelta)
	{
		if (!m_reciveInput) { m_positionChange = Ogre::Vector3::ZERO; m_yaw = 0; m_pitch = 0;return; }
		float MoveSpeed = 7.0f;
		float TurnSpeed = 0.1f;
		m_camera->moveRelative(m_positionChange * timeDelta * MoveSpeed);
		m_camera->yaw(Ogre::Radian(1)*m_yaw*timeDelta*TurnSpeed);
		m_camera->pitch(Ogre::Radian(1)*m_pitch*timeDelta*TurnSpeed);
		m_yaw = 0;
		m_pitch = 0;
	}
	void MovableCamera::ShouldReciveInput(bool State)
	{
		m_reciveInput = State;
	}
	bool MovableCamera::isRecivingInput()
	{
		return m_reciveInput;
	}
	Ogre::Camera * MovableCamera::getCamera()
	{
		return m_camera;
	}

	void MovableCamera::textInput(const SDL_TextInputEvent & arg)
	{
	}
	void MovableCamera::keyPressed(const SDL_KeyboardEvent & arg)
	{
		switch (arg.keysym.sym)
		{
		case SDLK_w:
			m_positionChange.z = -1;
			break;
		case SDLK_s:
			m_positionChange.z = 1;
			break;
		case SDLK_a:
			m_positionChange.x = -1;
			break;
		case SDLK_d:
			m_positionChange.x = 1;
			break;
		case SDLK_q: {
			m_camera->setPosition(Ogre::Vector3(1, 0, 15));
			m_camera->lookAt(Ogre::Vector3(0, 0, 0));
			break;
		}
		}
		
	}
	void MovableCamera::keyReleased(const SDL_KeyboardEvent & arg)
	{
		switch (arg.keysym.sym)
		{
		case SDLK_w:
		case SDLK_s:
			m_positionChange.z = 0;
			break;
		case SDLK_a:
		case SDLK_d:
			m_positionChange.x = 0;
			break;
		}
	}
	void MovableCamera::mouseMoved(const SDL_Event & arg)
	{
		m_yaw -= arg.motion.xrel;
		m_pitch -= arg.motion.yrel;
	}
	void MovableCamera::mousePressed(const SDL_MouseButtonEvent & arg)
	{
	}
	void MovableCamera::mouseReleased(const SDL_MouseButtonEvent & arg)
	{
	}
}
