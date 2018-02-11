#pragma once

#include <vector>
#include <memory>
#include <cassert>

namespace GEM
{
	class EngineController;
	class Service;

	/**!
	Yup. The GEM::Service
	It get's initialized, destructed, pre/.../post-framed.

	One more thing. Service is something, that work for entire game. They can be addressed,
	for example you can ask them to render something for you, or to send something over a network.
	They are not something, that is actual game. For that purpose, use screens.
	*/
	class Service
	{
	protected:

		enum ActionResult
		{
			AR_OK,///! Everything is fine.
			AR_ERROR///! Something's gone bad it we must terminate
		};

		virtual ActionResult initialize()=0;
		/**!
		All servicess must guarantee, that nothing bad will happen if shutdown gets called without initialize!
		*/
		virtual void shutdown() = 0;

		virtual ActionResult preFrame(float timeDelta)=0;
		virtual ActionResult frame(float timeDelta) = 0;
		virtual ActionResult postFrame(float timeDelta) = 0;


		inline EngineController* getEngineController() const {
			return m_owner;
		};

	private:
		EngineController* m_owner;
		inline void setEngineController(EngineController* ptr) {
			assert(m_owner == nullptr);/*setEngineController, apperantly, get's called more then once. That means, that multiple EngineControllers are trying
			to get one Service. Why is there even more then one EngineControllers?*/
		m_owner = ptr; }
		friend EngineController;

	public:
		inline Service() : m_owner(nullptr) {};
		inline virtual ~Service() {}
	};

	
	class EngineController
	{
	public:
		/**!
		\brief Starts entire engine

		It contains main loop and. actually, just everything.
		*/
		int start();

		/*!
		This method actualy replaces the constructor of GEM_Service. It tries to create new service and register it in a controller.
		\param[in] Controller A controller to which a service must be registered
		\returns Returns a pointer to created Service if succeed, and null if something gone wrong
		\note As you can see, it's a template parameter, so it's passes only a type of \c Service to register. So services cannot recive any parameters
		during creation that way. Actually, services do not need any parameters on initialization time. They now better, how they shold be created.
		*/
		template<class ServiceClass, typename ...Args>
		ServiceClass* AddService(Args ...args);


		/**!
		If we need to start shuting down after this frame, this method should be called.
		But remember, only after this frame. So don't call shutdown and then destroy all resources, that might be used someone later in the pipeline of this
		frame!
		*/
		void shutdown();

		/**!
		Sets the limit on maximum number of frames per second
		*/
		void setMaximumFPS(float fps);

	private:
		std::vector<std::unique_ptr<Service>> m_servicesVector;
		bool m_shouldTerminate = false;//Set to true, if it's time to shut down
		float m_frameSpeedLinit = 0.01f;//Limits the minimum time a full frame(pre-, -, post-) can be completed

		/**!
		Initializes all the services and return true. Services gets initialized in order they were regestered(FIFO).
		If one of services fails to initialize, initialization stops and false is returned.
		*/
		bool initializeServices();

		/**!
		Shuts all the services in opposite order. It doesn't know, if all the services were initialized, so it's just shuts them all.
		That's way services must be ready to recive shutdown, even if there wasn't any initialize. Who knows, maybe some other service will broke right before them?
		*/
		void shutdownServices();

		/**!
		Does a preFrame for all of the services. If some of them return something other then AR_OK, then everything stops, and false is returned,
		otherwise true gets returned. The same goes for doFrame and doPostFrame 
		*/
		bool doPreFrame(float TimeDelta);
		bool doFrame(float TimeDelta);
		bool doPostFrame(float TimeDelta);
		

	};

	template<class ServiceClass, typename ...Args>
	ServiceClass* EngineController::AddService(Args ...args)
	{
		auto ServiceObj = static_cast<Service*>(new ServiceClass(args...));
		ServiceObj->setEngineController(this);
		m_servicesVector.push_back(std::unique_ptr<Service>(ServiceObj));
		return static_cast<ServiceClass*>(ServiceObj);
	}
}