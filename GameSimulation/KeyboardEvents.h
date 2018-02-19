#pragma once
#include "EventBase.h"

/**!
Creates new KeyboardEvent class named KeyboardEvent_<x>.
This macro can only be used in this header! And, well while we're on it
all KeyboardEvents must be declared only in this header!
*/
#define NEW_KEYBOARD_EVENT(x) class KeyboardEvent_##x : public GEM::GameSim::KeyboardEventBase \
{\
	GAMESIM_EVENT(KeyboardEvent_##x) \
	public:\
	KeyboardEvent_##x(bool isPressed) : KeyboardEventBase(isPressed) {}\
}

namespace GEM::GameSim
{
	/**!
	Player input. It contains of several parts.
	Part 1 - EventMaker. This part transforms events from SDL2 to events of input system. The beauty of this part
	is that it's execution time configurable. Diffrent SDL2 event can correspond to different PlayerInput events
	and that correspondance can be altered dynamicaly throuh XML file.

	Next thing is EventTransmitter. EventTransmitter works with EventMaker and used to inject this events in a GameSimulation

	Then something in gameSimulation should apply this events to a right entity and it's internal mixins. To do that I think of
	expanding the interface of BaseEntity and BaseMixin. Each of them will recive new method

	HandleEvent(EventBase* event);
	The problem is to recognize this event. So every event will should have unique ID. I thing uint16_t should be enough.
	Then every event must specialize template method
	template<class T>
	T* Recognize(EventBase* event) that would return same pointer, but casted to a specified type, is the event actually of that type
	or return nullptr otherwise.

	Then every mixin in it's HandleEvent method. Wait. Mixins, probably, would prefer using switch statement for event recognision.
	So it'll be much better, if every Event would define some constexpr static constant publicly inside of a class, so that
	everyone would be able to recognize the type by this constant.

	Or even better
	template<typename T>
	constexpr EVENT_ID_TYPE getUniqueConstant()
	{
	return ConstexprHash(#classname);
	}

	But is it really better? I would say yes. For now on, both of this things would be implemented with a preprocessor macro
	that would insert the name of a class as a string for hash function.

	But wait. Templated method would, potentialy, prolong compilation. Becouse it will be instanciated for every compile unit and would
	be compiled the same way. But isn't the same will happen to a class definition in a header file?
	But still, in a header the would be only one function call, while in a templated version, every time specific ID is mentioned, there will be
	a call.

	So OK, let it be a constexpr static member.
	*/


	/**!
		KeyboardEvents is a subset of user input events, that represents some kind of a key weather its a keyboard key
		or a key on a mouse. It's a pretty common model for an event, so there is a special macro, that allows to build
		a KeyboardEvent class derivative with just one line!
	*/
	class KeyboardEventBase : public EventBase
	{
	public:
		/**!
		Key can be pressed or released, it's a pretty simple concept, actually.
		true - key is pressed.
		false - key is released.

		\note There are no guarantee, that after one KeyPressed event, can be only KeyReleased event. It's entirely depends
		on the source of the events.
		*/
		const bool m_isPressed;//true - pressed, false - unpressed
		KeyboardEventBase(bool isPressed) : m_isPressed(isPressed) {}
	};

	NEW_KEYBOARD_EVENT(PlayerMoveForward);
	NEW_KEYBOARD_EVENT(PlayerMoveBackward);
	NEW_KEYBOARD_EVENT(PlayerMoveLeft);
	NEW_KEYBOARD_EVENT(PlayerMoveRight);
	NEW_KEYBOARD_EVENT(PlayerJump);
	NEW_KEYBOARD_EVENT(PlayerAttack);

}
#undef NEW_KEYBOARD_EVENT