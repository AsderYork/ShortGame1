#pragma once
#include "KeyboardEvents.h"
#include <cereal\cereal.hpp>
#include <cereal\archives\binary.hpp>

//Events headeres
#include "KeyboardEvents.h"

namespace GEM::GameSim
{
	/**!
	EventSerializator is an internal mechanism, that allows serialization of a unique_ptrs of EventBase type
	in such a way, that will correctly transfer type.
	But of course it's not without a cost.

	To serialize an events, first of all, it MUST define<br>
	static std::unique_ptr<EventBase> deserialize(cereal::BinaryInputArchive& ar)<br>
	method. This method should create a unique_ptr with a type of that event whcih state is based on data from
	passed archive. 

	But that's not all! Then a special file should be created, that would contain series of new event definitions
	(see KeyboardEvents.def as an example). file should contain only this definitions and optionally comments, and nothing more!

	Then header with declared event should be added as include file (Somehere around "Events header" comment), and event definition file
	shold be added with #include directive in switch block of \c load method (see the position if #include "KeyboardEvents.def"). And
	only then the event is legit.

	You might wonder, why it's so complicated, but it's actually not THAT complicated. Perform dynamic type recognision and altering serialization
	functions based on that type is not easy by itself and this paricular solution require only 3 steps.
	1) Derive your event from EventBase obeing all the rules(Just use a macro to add ID and then declare serialization functions)
	2) Add header to beginning of this file
	3) Create definition file and add it to load block

	If there is multiple events of a similar type, that must be added, it is allowed to use the definition file in a header file and
	create all neccessary classes using macros. But don't forget to clean after yourself and add #undef NEW_EVENT at the and of a file.


	*/


	template<class Archive>
	void save(Archive & archive, std::unique_ptr<EventBase> const & m)
	{
		archive(m->getEventID());
		m->serialize(archive);
	}

	template<class Archive>
	void load(Archive & archive, std::unique_ptr<EventBase>& m)
	{
		EVENT_ID_TYPE id;
		archive(id);
#define NEW_EVENT(x) case x::id: {m = x::deserialize(archive); break;}
		switch (id)
		{
#include "KeyboardEvents.def"
		default: {m = nullptr; break; }
		}
#undef NEW_EVENT
	}
}