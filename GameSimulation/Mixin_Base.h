#pragma once

namespace GEM::GameSim
{
	/**!
	Base class for all mixins

	Entities in a game are constructed from various mixins, that provide entity's properties
	Every mixin must be derived from this class
	Every mixin also must declare
	"static const int MixinID" variable with same unique ID
	
	Mixin may provide some methods to alter the entity from outside of the simulation,
	in that case it must use Mixin_Controller singleton
	*/
	using MIXIN_ID_TYPE = int;

	class Mixin_base
	{
	public:
		/**!
		Called every simulation tick. Must return true, if everything is ok, false will terminate simulation, probably
		*/
		virtual bool tick(float delta) = 0;
		virtual ~Mixin_base() {};
	};
}