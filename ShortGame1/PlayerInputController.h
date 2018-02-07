#pragma once
/**!
So the problem is that player can controll various things in various ways.
Ok. Player controlls a character. What Else? A boat, for example.
But Boat uses pretty same controlls. So, like, we have a "move_forward" button
and we basicaly want it so that everything, that is moving, would respond to it in a same way.

The idea: every mixin can declare a method of controlls. Like "move_forward". Then PlayerInputController will map it to a proper
key. But this would work ONLY for a key presses! What about mouse movement?
Well, we can have a special action, called...
Wait a little, actions and listeners!

So all the actions must be named up front. Mixins cannot add new ones just like that. Then every mixin subscribes to an action
with something like

SUBSCRIBE_TO_ACTION(MixinClass, MethodID, ActionName);

And yes, parameter list of this function must match exactly.

Then whenever action occurs, PlayerInputControll sends a command to an entity. What entity? Well,
I Guess we should call it for all entities, that are marked as player's. And yeah, it's probably better to ditch this Player's entity list
and leave only one.

And yeah. It's and SDL2 thing. So let's move it there

*/
namespace GEM
{

	class PlayerInputControll
	{
	public:
		enum class Actions {
			MOVE_FROWARD_PRESSED,
			MOVE_FROWARD_RELEASED,

			MOVE_BACKWARD_PRESSED,
			MOVE_BACKWARD_RELEASED,

			MOVE_LEFT_PRESSED,
			MOVE_LEFT_RELEASED,

			MOVE_RIGHT_PRESSED,
			MOVE_RIGHT_RELEASED,

			MOUSE_MOVE,
			MOUSE_WHEEL_MOVE,

			MOUSE_ATTACK_PRESSED,
			MOUSE_ATTACK_RELEASED
		};



	};
}