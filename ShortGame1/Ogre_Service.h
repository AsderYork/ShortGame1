#pragma once
#include "EngineController.h"
#include <OGRE\OgreRoot.h>
#include <memory>
#include "OgreLogRedirection.h"
#include "SDL2_Service.h"

namespace GEM
{
	/**!
	This class allows controll of SDL. Creation of window, OS Messages processing and user input
	*/
	class Ogre_Service : public GEM::Service
	{
	public:
		virtual ActionResult initialize() override;
		virtual void shutdown() override;
		virtual ActionResult preFrame(double timeDelta) override;
		virtual ActionResult frame(double timeDelta) override;
		virtual ActionResult postFrame(double timeDelta) override;

		Ogre_Service(SDL_Controller* SDL);


	private:
		/*Ogre uses its own logging system. Won't tolerate that! This thing redirects ogre log messages to log4cpp, which is */
		GEM::OgreLogToLogForCpp m_ogreLog;

		std::unique_ptr<Ogre::Root> m_root;
		Ogre::RenderWindow* m_renderWindow;
		Ogre::SceneManager* m_sceneManager;
		Ogre::Camera* m_camera;
		Ogre::CompositorWorkspace* m_workspace;
		Ogre::SceneNode* m_node;

		SDL_Controller* m_sdlController;

		/**! 
		Retrives width and height from Ogre's config. If it can't get this values, 1280x720 get's returned.
		*/
		std::pair<int, int> getWidthHeighgtFromConfig();
		void chooseSceneManager();
		void createCamera();
		Ogre::CompositorWorkspace* createWorkspace();
		void CreateCube();
		void CreateCubeFromMesh();
		Ogre::IndexBufferPacked* createIndexBuffer();
		void RegisterHLMS();
	};

}