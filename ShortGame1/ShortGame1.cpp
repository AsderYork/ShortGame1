#include "stdafx.h"
#include "EngineController.h"
#include "SDL2_Service.h"
#include "Ogre_Service.h"

class MouseListener : public GEM::SDL_MouseListener
{
	void mouseMoved(const SDL_Event &arg) {}
	void mousePressed(const SDL_MouseButtonEvent &arg)
	{
		switch (arg.button)
		{
		case SDL_BUTTON_LEFT:
			LOGCATEGORY("Mouse!").warn("MouseButton was pressed! Left!");
			break;
		case SDL_BUTTON_RIGHT:
			LOGCATEGORY("Mouse!").warn("MouseButton was pressed! Right!");
			break;
		case SDL_BUTTON_MIDDLE:
			LOGCATEGORY("Mouse!").warn("MouseButton was pressed! Middle!");
			break;
		default:
			break;
		} 
	}
	void mouseReleased(const SDL_MouseButtonEvent &arg) {
		switch (arg.button)
		{
		case SDL_BUTTON_LEFT:
			LOGCATEGORY("Mouse!").warn("MouseButton was released! Left!");
			break;
		case SDL_BUTTON_RIGHT:
			LOGCATEGORY("Mouse!").warn("MouseButton was released! Right!");
			break;
		case SDL_BUTTON_MIDDLE:
			LOGCATEGORY("Mouse!").warn("MouseButton was released! Middle!");
			break;
		default:
			break;
		}
	}
};

class KeyboardListener : public GEM::SDL_KeyboardListener
{
	void textInput(const SDL_TextInputEvent& arg) 
	{
		LOGCATEGORY("TEXT").crit("Text:%s",arg.text);
	}
	void keyPressed(const SDL_KeyboardEvent &arg) {}
	void keyReleased(const SDL_KeyboardEvent &arg) {}
};



int main(int argc, char *argv[])
{
	MouseListener MLS;
	KeyboardListener KLS;

	GEM::EngineController Controller;
	auto SDLController = Controller.AddService<GEM::SDL_Controller>();
	auto OgreController = Controller.AddService<GEM::Ogre_Service>(SDLController);

//	SDLController->registerMouseListener(&MLS);
	//SDLController->registerKeyboardListener(&KLS);

    return Controller.start();
}

