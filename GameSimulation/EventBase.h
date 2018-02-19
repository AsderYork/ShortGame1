#pragma once
#include <cstdint>
#include <string>

/**!
Defines id field and virtual functions of class EventBase

Every non-pure-virtual event class definition, should start as<br>
class <event_name> : public <EventBase | <otherEvent>><br>
{<br>
GAMESIM_EVENT(<event_name>)<br>
\note ID is actually just a hash of a passed string. So watch out for collisions!
*/
#define GAMESIM_EVENT(x) public: constexpr static GEM::GameSim::EVENT_ID_TYPE id = FNVHash(#x);\
virtual GEM::GameSim::EVENT_ID_TYPE getEventID() const override {return id;}\
virtual std::string getEventTypeName() const override {return #x;}\
 private:

/**!
Provide 16bit hash of a string.
\note This function should, probably, be moved somewhere more general.
*/
constexpr uint16_t FNVHash(const char* string)
{
	uint16_t hash = 40389;
	while (*string)
	{
		hash = hash ^ (uint16_t)(*string++);
		hash = hash * 403;
	}

	return hash;
}

namespace GEM::GameSim
{
	/**!
	Type of ID's for events.
	Every non-pure-virtual event class MUST have<br>
	constexpr static EVENT_ID_TYPE id<br>
	field. It is recomended to use GAMESIM_EVENT(x) macro insert this field.
	*/
	using EVENT_ID_TYPE = uint16_t;

	/**!
	Events. So we have a GameSim. It's simulaties the game, but we don't want to just watch it progresses!
	It not a movie or a Game of Life we're doing here, we want to interact with the world! Affect it in some way or another.
	This affects are represented by events.
	Events are passed down to specific entities and to it's mixins. If mixin wants it can react to that event in some
	way, altering the state of the entity.<br>
	This is the base class for all the events.

	Implementation must guarantee, that every non-pure-virtual class, derived from this one have<br>
	constexpr static EVENT_ID_TYPE id<br>
	field with value, unique for every declared class. It is recomended to use
	GAMESIM_EVENT(<classname>) macro to define this field and base virtual methods.
	*/
	class EventBase
	{
	public:
		/**!
		Return unique ID of a type of this event

		Implementation must guarantee, that for any Event derived from this class,
		result of this method is equal to id field of that class.
		Just use GAMESIM_EVENT(<classname>) of beginning of a class definition.
		*/
		virtual EVENT_ID_TYPE getEventID() const = 0;
		/**!
		Return name of the eventType.

		Implementation must guarantee, that result is realy a name of that class.
		Just use GAMESIM_EVENT(<classname>) of beginning of a class definition.
		*/
		virtual std::string getEventTypeName() const = 0;
	};
}