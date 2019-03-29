#pragma once
#include <SDL2/SDL_keycode.h>

namespace GEM {
	
	struct InputEvent_MouseState
	{
		int curr_x = 0, curr_y= 0 , rel_x = 0, rel_y = 0;
		int rel_wheel=0;

		bool leftButton = false, rightButton = false, middleButton = false;

	};


	struct InputEvent_Button
	{
		SDL_Keycode code;
		bool is_pressed;

		InputEvent_Button(SDL_Keycode _code, bool _is_pressed) : code(_code), is_pressed(_is_pressed) {}
	};

}