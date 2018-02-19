#pragma once
#include <cereal\cereal.hpp>
#include "EventBase.h"

#define MIXIN_ID(id) static const MIXIN_ID_TYPE MixinID = id; inline virtual const MIXIN_ID_TYPE getMixinID() const override{return MixinID;}

namespace GEM::GameSim
{
	/**!
	Base class for all mixins

	Entities in a game are constructed from various mixins, that provide entity's properties
	Every mixin must be derived from this class
	Every mixin also must declare
	"static const int MixinID" variable with same unique ID
	This one is for class recognision during entity building. But then we need to address this mixin in an entity
	for that there is virtual method that also should be overloaded and must return same ID
	It's adviced to use special macro to handle all that stuff
	
	Mixin may provide some methods to alter the entity from outside of the simulation,
	in that case it must use Mixin_Controller singleton
	*/
	using MIXIN_ID_TYPE = int;

	class Mixin_base
	{
	public:

		virtual const MIXIN_ID_TYPE getMixinID() const = 0;

		/**!
		Every tick simulation asks every mixin of every entity, if it feels like it needs to send some data
		to clients. Actually, map is big, and some updates should be sent only to clients, that, really needs them
		For example distance. So we want to send updates of entities to a player, only if it close enough.
		Well, we can do that later, actually. We can do a special cycle for all the players, and then, based on their 
		possition, we can ither retranslate them that changes, or send them command to despawn entity.
		Ok, but what if entity just entered player's field of view? Technicly speaking, we can track this.
		We just need to know, if player been tracking this entity. And now player have this set.
		Ok, lets for example take Mixin_Movable. So it has position and speed.
		And, basicaly speaking, it should be updated untill speed is zero.
		When it reched zero, it must emit one last time and then stop.
		Is it possible?
		Yes. With this stuff it is possible. But how Update should work?
		So we run over every mixin in every entity, and after tick(). How about we just make every mixin avaliable for 
		archivation?
		In that case, we can just pack mixin into an archive, send it over, and basically replace the data on the other side!
		Totaly good idea!


		This method is call AFTER every tick. If mixin returns true with it,
		it will be packed with cereal and become ready to translation over to clients.
		*/
		virtual bool NeedsUpdate() {
			return false;
		}

		/**!
		There are various types of updates so entity could send only useful data, when possible
		*/
		enum class UpdateReason {
			APPEAR,//!Entity just entered player's view. So mixin should send everything, becouse on the other side it totaly not configured!
			REGULAR//!Entity remains in player's view, so mixin can relay on previous updates and send only neccessary data.
		};
		virtual void SendUpdate(cereal::BinaryOutputArchive& archive, const UpdateReason reason) = 0;

		/**!
		This one for clients. Entity on a server can alter it state, so client's entity should be altered in a same way.
		Beware of various update scheems!
		*/
		virtual void ReciveUpdate(cereal::BinaryInputArchive& archive) = 0;

		/**!
		Called every simulation tick. Must return true, if everything is ok, false will terminate simulation, probably
		*/
		virtual bool tick(float delta) = 0;

		/**!
		Recives an event, that were sent to the entity, that this mixin is part of. It's up to mixin to check the
		id of that event and then decide whether it will react on that event in some way, or just ignore it.

		\note Technicaly mixins may want to send an event as a reaction to some other event. This event will be placed
		in the queue and processed on the same frame, so beware of infinite loops, were two or more mixins exchange events indefinitely.
		*/
		virtual void ReciveEvent(const EventBase * const event) {};
		virtual ~Mixin_base() {};
	};
}