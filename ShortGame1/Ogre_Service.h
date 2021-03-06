#pragma once
#include "EngineController.h"
#include <OGRE\OgreRoot.h>
#include <memory>
#include "OgreLogRedirection.h"
#include "SDL2_Service.h"

namespace GEM
{
	class Ogre_Service;

	/**
	Temporary class. Allow camera to be moved with mouse and keys!
	*/
	class MovableCamera : public SDL_KeyboardListener, public SDL_MouseListener
	{
	public:
		//crates the camera; Call in Initialize of OgreService
		void SetCamera(Ogre::SceneManager* sceneManager, SDL_Controller* SDLController);
		//Applys changes to a camera position. Call in preframe
		void AjustPosition(float timeDelta);

		/**
		\brief Controls wether camera should recive input, or should ignore it
		\param[in] State - true for reciving input, false for ignoring it
		*/
		void ShouldReciveInput(bool State);
		bool isRecivingInput();

		Ogre::Camera* getCamera();

	private:
		friend class Ogre_Service;
		Ogre::Camera* m_camera;
		Ogre_Service* m_service;
		bool m_UseThisCamera = false;
		Ogre::SceneNode* m_debugCameraSceneNode;


		//Change of position and rotation since last frame;
		Ogre::Vector3 m_positionChange = Ogre::Vector3::ZERO;
		float m_yaw = 0;
		float m_pitch = 0;
		bool m_reciveInput = true;

		// ������������ ����� SDL_KeyboardListener
		virtual void textInput(const SDL_TextInputEvent & arg) override;
		virtual void keyPressed(const SDL_KeyboardEvent & arg) override;
		virtual void keyReleased(const SDL_KeyboardEvent & arg) override;

		// ������������ ����� SDL_MouseListener
		virtual void mouseMoved(const SDL_Event & arg) override;
		virtual void mousePressed(const SDL_MouseButtonEvent & arg) override;
		virtual void mouseReleased(const SDL_MouseButtonEvent & arg) override;
	};


	/**!
	This class allows controll of SDL. Creation of window, OS Messages processing and user input
	*/
	class Ogre_Service : public GEM::Service
	{
	public:
		virtual ActionResult initialize() override;
		virtual void shutdown() override;
		virtual ActionResult preFrame(float timeDelta) override;
		virtual ActionResult frame(float timeDelta) override;
		virtual ActionResult postFrame(float timeDelta) override;

		Ogre_Service(SDL_Controller* SDL);

		Ogre::Root* getRoot();
		Ogre::Camera* getCamera();

		void AllowCameraMovement(bool State);
		bool isCameraAllowedToMove();

		void ResetCompositor(std::string CompositorName = "MainWorkspace");


	private:
		/*Ogre uses its own logging system. Won't tolerate that! This thing redirects ogre log messages to log4cpp, which is */
		GEM::OgreLogToLogForCpp m_ogreLog;

		std::unique_ptr<Ogre::Root> m_root;
		Ogre::RenderWindow* m_renderWindow;
		Ogre::SceneManager* m_sceneManager;
		Ogre::CompositorWorkspace* m_workspace;
		Ogre::SceneNode* m_node;
		MovableCamera tmpCamera;
		SDL_Controller* m_sdlController;

		/**! 
		Retrives width and height from Ogre's config. If it can't get this values, 1280x720 get's returned.
		*/
		std::pair<int, int> getWidthHeighgtFromConfig();
		void chooseSceneManager();
		void createCamera();
		Ogre::CompositorWorkspace* createWorkspace();
		//void CreateCube();
		//void CreateCubeFromMesh();
		Ogre::IndexBufferPacked* createIndexBuffer();
		void RegisterHLMS();
		void setupResources();
	};

}