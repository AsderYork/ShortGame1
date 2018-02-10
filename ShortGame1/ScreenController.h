#pragma once
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
	public:
		
	};

	class ScreenController
	{
	private:
		std::list<std::unique_ptr<Screen>> m_screens;
	public:

		
	};


}