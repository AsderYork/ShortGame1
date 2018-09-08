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
	
	\li \c ServicesReady. Most of the screens will be registered in the \main before the GameEngine even starts working, whcih means that
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
	class ScreenController;

	/**
	Describes screen's interface. More on it in \ref DescScreens.
	*/
	/*class NewScreen 
	{
	private:	

	public:

	};

	/**
	Controlls game screen. More on it in \ref DescScreens.
	*/
	/*class ScreenService : public Service
	{
	private:
		/**
		Now screens are registered once and for all the lenght of the game.
		So this vector holds 
		*/
		/*std::vector<std::unique_ptr<Screen>> m_registeredScreens;
		std::vector<Screen*> m_stack;

	public:
		
		RegisterScree

	};*/
	
	/**
	Represent current game state and may have some hooks for CEGUI, SDL2, SOUND
	*/

	class Screen
	{
	private:
		ScreenController * m_controller;
		bool m_shouldBeDeleated = false;
		friend class ScreenController;


	protected:
		inline ScreenController * getController() { return m_controller; }

		/**
		This method is called once after registration and only after all services have been initialized.
		So do all your initialization there.
		*/
		virtual void Init() {};

		/**
		This method is called when screen gets on top of the stack
		It might be called multiple times if other screens are geting created and then deleated in front of this one.
		*/
		virtual void IsOnTop() {};

		/**
		If there is a need for a regular execution, Screens can access post-frame
		*/
		virtual void PostFrame(float timeDelta) {};

		/**
		If screens finished all its job and is need no more, it should call
		this method, to mark itself for deltition. After this call, any use of this instance
		is prohibited.
		*/
		void Finish();

		virtual void textInput(const SDL_TextInputEvent & arg) {};
		virtual void keyPressed(const SDL_KeyboardEvent & arg) {};
		virtual void keyReleased(const SDL_KeyboardEvent & arg) {};


		virtual void mouseMoved(const SDL_Event & arg) {};
		virtual void mousePressed(const SDL_MouseButtonEvent & arg) {};
		virtual void mouseReleased(const SDL_MouseButtonEvent & arg) {};

	public:

		virtual ~Screen() {};
		
	};

	class ScreenController : public Service, SDL_KeyboardListener, SDL_MouseListener
	{
	private:
		std::list<std::unique_ptr<Screen>> m_screens;

		void CleanListUp();

		bool m_initialized = false;

	public:

		ScreenController(SDL_Controller* SDL);

		template<class T, typename... Args>
		T* AddScreen(Args&&...args)
		{
			auto Ptr = std::make_unique<T>(std::forward<Args>(args)...);
			Ptr->m_controller = this;
			if (m_initialized) { Ptr->Init(); }
			return dynamic_cast<T*>(m_screens.emplace_back(std::move(Ptr)).get());
		}




		
		virtual void textInput(const SDL_TextInputEvent & arg) override;
		virtual void keyPressed(const SDL_KeyboardEvent & arg) override;
		virtual void keyReleased(const SDL_KeyboardEvent & arg) override;


		virtual void mouseMoved(const SDL_Event & arg) override;
		virtual void mousePressed(const SDL_MouseButtonEvent & arg) override;
		virtual void mouseReleased(const SDL_MouseButtonEvent & arg) override;


		virtual ActionResult initialize() override;
		virtual void shutdown() override;
		virtual ActionResult preFrame(float timeDelta) override;
		virtual ActionResult frame(float timeDelta) override;
		virtual ActionResult postFrame(float timeDelta) override;

	};


}