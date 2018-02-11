#pragma once

#include "EngineController.h"
#include "SDL2_Service.h"

#include <list>
#include <memory>

namespace GEM
{
	/**!
	Screens. They are the means of controlling what is shown on a screen, what recives user input and
	overall - in what state the game is. Like, is it connected to a server? Did we load the
	game entities and landscape? Are we showing some in-game menu? This type of things.
	
	Every screen have three important parts - CEGUI, SDL2, SOUND. At the moment of writing there is no sound system
	so we just assume it will work like the other two listed above.

	ScreenController is responsible for creating stack-like structure for screens, so that a screen on top could block
	input for other screens. It can also be used to controll the entire game state like with the case of transition between
	Client connection screen to a game screen.
	*/
	class ScreenController;

	/**!
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

		/**!
		This method is once after registration and only after all services have been initialized.
		So do all your initialization there.
		*/
		virtual void Init() {};

		/**!
		If there is a need for a regular execution, Screens can access post-frame
		*/
		virtual void PostFrame(float timeDelta) {};

		/**!
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