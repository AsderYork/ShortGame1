#pragma once
#include <vector>
#include <memory>
#include <chrono>
#include "OIS_Input.h"
#include "GUI_Console.h"
#include "GEMCDS_Connection.h"
#include "GUITool.h"

namespace GEMClient
{
	/*!
	\brief Represents a module, that need to be called every frame

	This class represents a service. Service - is something, that performs work every frame, and what have to be initialized and shuted down somehow.
	Audio, Graphis, Simulation, Input, Netwok. Something like that
	*/
	class GEM_Service;

	/*!
	\brief Controlls all the services and tools in the project. So it's basically a main loop.

	This is a controller of Services. It performs all the initialization frame-management and halting of services and tool.
	This class is basicaly grabs \c main's flow	and then the game get's started!
	*/
	class Service_Controller;

	class Service_Controller {
	public:
		//!Constructor. Nothing get constructed on the start
		Service_Controller();
		//!Desctructor. Everything have to be deconstucted before call of destructor, or use RAII-envelopes(like unique_ptr) to be deconstructed automatically
		~Service_Controller();

		/*!
		This method actualy replaces the constructor of GEM_Service. It tries to create new service and register it in a controller.
		\param[in] Controller A controller to which a service must be registered
		\returns Returns a pointer to created Service if succeed, and null if something gone wrong
		\note As you can see, it's a template parameter, so it's passes only a type of \c Service to register. So services cannot recive any parameters
		during creation that way. Actually, services do not need any parameters on initialization time. They now better, how they shold be created.
		*/
		template<class ServiceClass>
		GEM_Service* AddService();

		/*!
		This function must be called in \c main after all services have been registered. It inits all the services in order they have been added
		and then starts main loop, untill any of services decided to stop. Then Shutdoun will be called for every service.
		\returns Returns a reason of terminition; They described in \c TERMINATE_REASON
		*/
		int Start();


		enum TERMINATE_REASON {
			TR_GOOD = 0,//<Everything is fine. User just stopped playing and terminate normaly
			TR_INIT = 1,//<One of the services have thrown an error, during the initialization
			TR_PREFRAME = 2,//<One of the services have thrown an error, during the pre-frame
			TR_FRAME = 3,//<One of the services have thrown an error, during the frame
			TR_POSTFRAME = 4//<One of the services have thrown an error, during the post-frame
		};

		/*!
		If this function is called, Controller will start to shuting down everything, as soon as it's finishes current frame.
		This is a normal way to terminate
		*/
		void Shutdown();

		/*Tools. They are here for all to use.*/
		OIS_Input Input;
		GUI_Console Console;
		GUITool m_GUITool;
		GEM::TLSClientContext TLS_Context;
		GEM::NetworkClient GEMCDS_Network_Client;


	private:
		friend GEM_Service;	

		/*! \brief Starts all the tools
		The GEM-thing consists of Services and Tools. Services usually work every frame, while tools get's inited and works in background
		Tools cannot expect Services to be inited at time they starts. Services, on the other hand, can expect all Tools to be loaded then they get started
		Services can use Tools directly, Tools can't do that and can be only used
		\returns Returns true if all tools started successfuly, false otherwise*/
		bool StartTools();


		/*!
		A vector of all the registered services;
		*/
		std::vector<std::unique_ptr<GEM_Service>> ServicesVector;

		/*!
		A flag for a peacful shutdown
		*/
		bool ShutdownFlag = false;

	};

	class GEM_Service
	{
	public:	
		
		virtual ~GEM_Service();	
		/*!
								Constructor is protected, so that only Controller could take ownership over Service, and becouse if it would be private, even inherited class can't access them*/
		GEM_Service();
	
	protected:

		/*!
		Returns a controller, so that direvative class can also access it
		\returns pointer to a controller, to which this service is registered
		*/
		Service_Controller* getController();

		/*!
		Preforms all of the initialization of a given Service. This method get's called by Controller before PreFrame, Frame, PostFrame, and Shutdown
		The Services will be initialized in order they were registered.
		\returns Returns true if init was successfull, and false if something gone wrong and Service must terminate. When one of the Services returns false,
		all services, that have been inited before this one, including this one would be Shutdown. The services, which havn't been inited, would not recive a shutdown
		*/
		virtual bool Init()=0;

		/*!
		This function get's called every frame, before Frame and PostFrame
		\param[in] Time Time since last frame.
		\returns Returns true if everything is fine and false, if somethings gone wrong and Controller must terminate
		*/
		virtual bool PreFrame(double Time)=0;

		/*!
		This function get's called every frame, before PostFrame and after PreFrame
		\param[in] Time Time since last frame.
		\returns Returns true if everything is fine and false, if somethings gone wrong and Controller must terminate
		*/
		virtual bool Frame(double Time) = 0;

		/*!
		This function get's called every frame, after Frame and PostFrame
		\param[in] Time Time since last frame.
		\returns Returns true if everything is fine and false, if somethings gone wrong and Controller must terminate
		*/
		virtual bool PostFrame(double Time) = 0;

		/*!
		This function gets after Init, so that inited Service can shut itself down.
		*/
		virtual void Shutdown() = 0;

	

	private:	
		


		friend Service_Controller;

		Service_Controller* controller;

	};

	template<class ServiceClass>
	GEM_Service * Service_Controller::AddService()
	{
		auto Service = static_cast<GEM_Service*>(new ServiceClass());
		Service->controller = this;
		Service_Controller::ServicesVector.push_back(std::unique_ptr<GEM_Service>(Service));	
		return Service;
	}

}