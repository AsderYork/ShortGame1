#pragma once
#include "EntitiesBase.h"
#include "Mixin_Objecttype.h"

namespace GEM::GameSim
{
	/**
	\page ObjectsAndEntities Objects and Entities
	A mixin incapsulates one aspect of an object's state. They also provide basic prediction and altering methods.
	An Entity is a collection of mixins, that together forms complete object's state. Entities also provide a unique ID's for evry object.
	Object is an Entity, that also have some specific behaviour.
	Objects can act on each other or on outside systems like landscape. This acting is representing by an event.
	So an objects is a state, that represented by underlying entity from one side and behavior, manifested in it's interractions
	with other objects and systems, represented by a set of events that this object can generate.

	Let's say a player wants to dig a hole in a landscape. This is what will happen:
	[Client's side]
	Users enters some input.
	GamePlayerControlls determines, that this is a request to dig a hole.
	PlayerObject assigned to this player recives this request and performs checks, if it is possible to do this.
	These checks might involve checks of internal state and queries to other systems(landscape system in this example).
	If some of this queries signalized, that this action cannot be performed, PlayerObject signalizes that to the caller(Which means that caller must wait at least one frame!)
	Otherwise player sends this querry to the server. Depending on the type of the action, the specified action might be performed right away
	with the provided ability to revert changes if needed, or the action might be performed only after positive server's respond(Which probably allready will contain altered GameSim state)
	[Server's side]
	When a server recives an event, it checks, if that aven is realy valid. Every event must somehow prove, that it realy could took place in
	within GameSim's rules, otherwise it will be discarded.
	If an event wasn't discarded, it will be applied to a gamestate. To prevent multiple applyment of the same change for a player, that sent that event
	changes must be somehow linked to it.

	*/

	/**\brief A base class of GameObject.
	More on what GameObject is in \ref ObjectsAndEntities
	\tparam Mixins Mixins, that should contain object's state.
	*/
	template<typename...Mixins>
	class GameObject : public MixedEntity<Mixin_Objecttype, Mixins...>
	{	
	};
}
