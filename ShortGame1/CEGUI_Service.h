#pragma once
#include "EngineController.h"
#include "Ogre_Service.h"
#include "SDL2_Service.h"
#include <vector>

namespace GEM
{
	class CEGUI_Service;
	
	class CEGUI_Layout
	{
	public:
		/**!
		\brief Loads and creates all the necessary resources, to make layout usable
		\returns Returns false, if resources can't be loaded
		*/
		virtual bool Initialize()=0;		
		/**!
		\brief Orders Layout to turn itself on
		*/
		virtual void turnOn()= 0;
		/**!
		\brief Orders Layout to turn itself off
		*/
		virtual void turnOff() = 0;
		/**!
		\brief If layout want's to do some changes, it should do them in this method.
		This method get's called in PreFrame if CEGUI_Service;
		*/
		virtual void PreFrame(float delta) = 0;

		virtual ~CEGUI_Layout() {};
		

	protected:
		/**
		flase if turn off. True if turn on;
		By default layouts is turned on initially. But if some layout wants to be on from the beginning, it MUST set this flag
		*/
		bool m_turnedOn = false;


	private:
	};

	class CEGUI_Service : public Service
	{
	public:
		CEGUI_Service(Ogre_Service* ogreService, SDL_Controller* sdlController);

		// Унаследовано через Service
		virtual ActionResult initialize() override;
		virtual void shutdown() override;
		virtual ActionResult preFrame(double timeDelta) override;
		virtual ActionResult frame(double timeDelta) override;
		virtual ActionResult postFrame(double timeDelta) override;

		template<class LayoutClass, typename ...Args>
		CEGUI_Layout* AddLayout(Args ...args);

	private:
		Ogre_Service* m_ogreService;
		SDL_Controller* m_sdlController;

		std::vector<CEGUI_Layout*> m_layoutsVector;

	};

	template<class LayoutClass, typename ...Args>
	CEGUI_Layout* CEGUI_Service::AddLayout(Args ...args)
	{
		auto LayoutObj = static_cast<CEGUI_Layout*>(new LayoutClass(args...));
		m_layoutsVector.push_back(LayoutObj);
		return static_cast<LayoutClass*>(LayoutObj);
	}

}