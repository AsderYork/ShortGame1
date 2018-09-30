#pragma once
#include <cereal\cereal.hpp>
#include "EntityIDType.h"
#include "GameTime.h"

/**!
Now we have history on client, but it works only for a client's character and only for it's movement.
Tests showed, that the way other characters moves looks really jagged becouse of this constant updates
and the time difference. The idea is to store every event that is happening in history, make methods
isStateChanged(), getStateChange(), applyStateChange() for every mixin. And so every time when
state of a mixin in an entity is changed on the server, it will send Update for this entity(btw, Update system
will be removed and complitely replaced with that one). Wait a minute, we allready have NeedsUpdate(), SendUpdate() and ReciveUpdate()!
It all the methods we need! The only thing missing is that notion of a state in mixin.
But how can this help our problem? First of all, why do we have this problem? Well, the hypothesis is that player when object
starts moving, it gain velocity, then for the next couple of frames it is moved by the GameSim.Tick() on the player, and then it recives
new Update from the server, and this update teleports the object back. Let's check it first, shall we?
So tests did cofirm the expectation. Server sends updates with the velocity pointing forward but the actual position
is behind the one, that the object allready had.
Ok, let's assume the model, where we're holding previous state and can compare it with the one we had allready.
We recive new update and we see, that it's behind us. Actually, if we're also knew, the time lag of this update, it could have halped us.
For example if we know, that update is lagging a little and we see the velocity, that is written in it, then we could just take check
if the velocity is the same, and if it, then we could just check, if veloctiy*lag=curr_pos. If it is, then this update is truly outdated and we
can safely ignore it. But we still have two bad cases: teleport and stopping.
When player stops, it still will be moved by a server update, becouse server desides, where you actually stop.
Teleport also would work ok, even if velocity is the same, we can clearly see, that position is not.

So ok, let the be a shared time, a 64-bit usigned integer value, so that 100000 uints of this time is in one seconds.
Quick calculation showed, that to avoid overflow problems, server must be restarted every 5.4 million years.
*/

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

	//Forward declarations
	class GameSimulation;
	template<typename...T>
	class EntityGenerator;


	/**!
	Represents a dedicated part of an entitie's state.
	For every aspect of every entity there must be a mixin, representing that mixin.
	*/
	class Mixin_base
	{
		template<typename... T>
		friend class EntityGenerator;
	private:
		ENTITY_ID_TYPE m_EntityID;
		GameSimulation * m_GameSim;
	protected:
		//Shows current time of the simulation
		inline const GameSimulation* getGameSim() { return m_GameSim; }
		inline const ENTITY_ID_TYPE getEntityID() { return m_EntityID; }
	public:
		
		const GameTime& getGameTime();

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
		Checks if update is correct and if it is, applies it.
		\param[in] archive An archive, that contains new mixin state
		\param[in] UpdateLag A time correction based on a lag between client and server
		\returns Returns true if update is correct and applied, returns false if update is incorrect.
		\note, update might be skiped if UpdateLag calculation shows, that new state is somehow close to a current one
		with respect to time lag. In that case true will be returned
		*/
		virtual bool CheckAndReciveUpdate(cereal::BinaryInputArchive& archive, const GameTime UpdateLag) = 0;

		/**!
		Applies event to a mixin. Unlike \c ReciveUpdate this method MUST apply the update as-is, without any tests.
		*/
		virtual void ApplyUpdate(cereal::BinaryInputArchive& archive) = 0;

		/**!
		Called every simulation tick. Must return true, if everything is ok, false will terminate simulation, probably
		*/
		virtual bool tick(const GameTime delta) = 0;

		virtual ~Mixin_base() {};
	};
}