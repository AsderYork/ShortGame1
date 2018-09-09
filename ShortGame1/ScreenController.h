#pragma once

#include "EngineController.h"
#include "SDL2_Service.h"

#include <vector>
#include <memory>

//ToDelete
#include <list>




namespace GEM
{
	/**
	\page DescScreens Screens and a screen service

	\par Related classes
	\li \c Screen
	\li \c ScreenService

	\par Description
	Screens. They are the means of controlling what is shown on a screen, what recives user input and
	overall - in what state the game is. Like, is it connected to a server? Did we load the
	game entities and landscape? Are we showing some in-game menu? This type of things.

	ScreenService is responsible for creating stack-like structure for screens, so that a screen on top could block
	input for other screens. It can also be used to controll the entire game state like with the case of transition between
	Client connection screen to a game screen.

	The lifetime of the screen is pretty simple:
	\li \c screen's \c constructor. First they are added to the ScreenService.
	At this state it is adviced to keep the resource consumption to the minimum. On the creation
	screen may take ptrs on services they depend on through constructor, but it's they still will be unitilialized, so it's adviced to just
	keep them for later use.
	
	\li \c Init. Most of the screens will be registered in the \main before the GameEngine even starts working, whcih means that
	screen, that do want to pre-load their resources in the background woldn't know, when to start, if this process depends on Services.
	To deal with this problem, this method is called for every registered state on ScreenController's \c init phase and for every screen
	right after the're added to the Controller.

	\li \c OnTop. It's important to understand, that when Screen is added to the ScreenConstructor, it's still not on the stack. It's just
	avaliable for other screens or outside users to add this screen on top of the stack. And only after this request is made, a screen
	is finally added on top! When a screen is on top, it's responsible for interacting with user. Which usually means, that all it's
	resources must be loaded as soon as it enters this stage. 
	But screen can appear on top not only on it's creation, but also when a screen, that was overlaping this one is no longer
	overlaps this one. So be careful with resource loading if your resources are supposed to exist even after an overlap.
	The state of the screen after this method is called and before state changes is called \c Visible

	\li \c NoLongerOnTop. This phase begins when a screen is overlapped by another screen. During that phase screen should unload most
	of it's resources.

	\li \c LeaveStack. A screen can decide to no longer be on a stack. It can't be removed immideately, becouse there's a game
	going on and it might be the middle of the frame, but at the end of the frame ScreenController will remove screen from the queue after
	calling \c LeaveStack method of the screen. During this method screen is supposed to unload any other resources, that is still have.

	As soon as screen have recived \c OnTop and before it changes state, it will recive frame events where it can do whatever it wants basically.

	\par Transparency
	So yeah. When a screen is added on top of the stack it hides the screen beneath it forcing it to be \c NoLongerOnTop. But
	what if we want to add a new screen but, somehow, also allow a screen beneath it to recive frame events? To solve this probem,
	a screen might declare itself as \transparent. Transparent screens will not hide the screens below them, allowing them to recive
	frame events and not causing them to recive \c NoLongerOnTop, but would still, themselves, recives frame events and \c OnTop call when added
	to the stack.

	So a scren is Visible (A state right after \c OnTop and before it's \c NoLongerOnTop or \c LeaveStack where it recives frame events) if it's on top
	of the stack, or if every screen above it is \c transparent. Which means that transparaty stacks.


	\note There was a long and difficult decision making process to decide what is the best way for screen to controll it's resources.
	There options was the one described above and a RAII-style variant, where every screen register it's constructor, evleloped in lambda
	to catch any needed Services, and then during the game, other screens can call this lambda, associated with their name which would create
	a new screen. At the end the first option where chosen, becouse it allows more controll over execution for screens and doesn't require
	arcanic mechanisms, like lambda registration.

	\note Screens should be unique in a stack in a scense, that maximum amount of screens in a stack is equal to the amount of screens
	 being registered, otherwise all that Stateful thing would lose any sence.

	*/

	class ScreenService;

	/**
	Describes screen's interface. More on it in \ref DescScreens.
	*/
	class Screen 
	{
	private:	
		friend class ScreenService;

		bool m_isTransparent;
		bool m_markedForRemoval;
		ScreenService* m_service;
	public:

		/**
		\param[in] isTransparant Determines if this screen is \c Transparant. More on it in \ref DescScreens
		*/
		inline Screen(bool isTransparant = false) : m_isTransparent(isTransparant), m_markedForRemoval(false), m_service(nullptr) {}
		virtual ~Screen() {}

	protected:

		/**
		\brief Marks a screen for removal from stack
		A screen can't be deleated just like that. It can only be removed between frames. Call this method
		to ask ScreenService, that holds this screen to remove it on the next opportunity.
		\note Screen is removed only from the stack. It will recive \c NoLongerOnTop if it was on top
		and \c LeaveStack, but it's instance would remain alive untill the ScreenService would get \c Unloaded.
		*/
		inline void MarkForRemoval() { m_markedForRemoval = true; }

		/**
		\brief Activates screen with provided ID.
		This method just provides easier access to ScreenService's \c ActivateScreen
		*/
		void ActivateAnotherScreen(int32_t id);

		/**
		\brief Start of the init phase
		This method is called on registered screens when ScreenService is getting \c initialized.
		\note If a screen is added to an allready initialized ScreenService, it will be \c Inited during it's registration.
		*/
		virtual bool Init() { return true; };

		/**
		\brief Called when a screen is on top of an inited queue.
		*/
		virtual void OnTop() = 0;

		/**
		\brief Called for a screen, that is no longer \c visible.
		Screen can lose \c Visibility ither by being overshadowed by another screen
		or when it lefts the stack.
		\warn DO NOT add other screens from this method!
		*/
		virtual void NoLongerOnTop() = 0;

		/**
		\brief Called for a screen that have left the stack
		Screens can require to leave the stack by calling \c MarkForRemoval, but int won't couse immidiate removal.
		This method is called right before when the screen is actually leaving the stack.
		\warn DO NOT add other screens from this method!
		*/
		virtual void LeaveStack() = 0;

		/**
		\brief Called for \c visible Screens on Game's \c PreFrame
		*/
		virtual void PreFrame(float Delta) {}

		/**
		\brief Called for \c visible Screens on Game's \c PostFrame
		*/
		virtual void PostFrame(float Delta) {}

	};

	/**
	\brief Controlls game screen. More on it in \ref DescScreens.
	*/
	class ScreenService : public Service
	{
	private:
		/**
		Now screens are registered once and for all the lenght of the game.
		So this vector holds 
		*/
		std::vector<std::pair<int32_t,std::unique_ptr<Screen>>> m_registeredScreens;
		std::vector<Screen*> m_stack;

		bool m_isInitted = false;

	public:
		
		/**
		\brief Registeres new screen.
		\tparam ScreenType  A screen type, that should be added in a service
		\tparam Params  Param types that will be transfered to Screen's constructor
		\param[in] id ID of the screen. Must be unique for every in a service!
		\param[in] Params Params, that will be transfered to a Screen's constructor
		\note It is adviced to use Hasher to name Screens with a logical names in code, but translate this names
		in int32_t on compile time
		*/
		template<typename ScreenType, typename... Params>
		void RegisterScreen(int32_t id, Params&& ...params)
		{
			for(auto& screen : m_registeredScreens)
			{
				if (screen.first == id)
				{
					LOGCATEGORY("ScreenService/RegisterScreen").crit("Can't register screen with id:%i; It's allready registered!", id);
					throw std::exception("Can't register screen");
				}

			}

			m_registeredScreens.emplace_back(id, std::make_unique<ScreenType>(std::forward<Params>(params)...));
			m_registeredScreens.back().second->m_service = this;

			if (m_isInitted)
			{
				if (!m_registeredScreens.back().second->Init())
				{
					LOGCATEGORY("ScreenService/RegisterScreen").error("Init stage of a screen #%i (init-on-register) returned false.", id);
					throw std::exception("Init stage of a screen (init-on-register) returned false.");
				}
			}
		}

		/**
		\brief Adds screen with a given ID on top of the stack.

		\note For now there is no conclusive argument on what should happen if this method is called
		with id of a screen, that is allready in stack. There is several possible outcomes
		\li Throw an exception. This makes sence, becouse if a behaviour is uncertain, it really looks like an error

		\li Do nothing.  This is like previous option, but wthout introducing implicing dependecy between callers,
		that would have to know about other calls to avoid causing error.

		\li Remove screen from where it is now and move it on top of the stack. This is like previous option, but assumes
		that if a call is done, it should be executed.

		For now the last option is implemented
		*/
		void ActivateScreen(int32_t screenId);

	protected:
		virtual ActionResult initialize() override;
		virtual void shutdown() override;
		virtual ActionResult preFrame(float timeDelta) override;
		virtual ActionResult frame(float timeDelta) override;
		virtual ActionResult postFrame(float timeDelta) override;

	};


}