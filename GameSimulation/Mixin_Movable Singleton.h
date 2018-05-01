#pragma once
#include "LandscapePhysics.h"

namespace GEM::GameSim
{
	class Mixin_Movable;

	/**!
	Yup. It's a singleton. This singleton provides a way for Mixin_Movables to interract with 
	other systems, such as LandscapePhysics without the need to somehow pass those systems as
	parameters into the Mixin_Movable instances.
	
	\note This is a recomended way of providing outside data to mixins for now. It's definetly bad that
	this solution is so obscure, but right now it's the best a can think of.
	*/
	class Mixin_Movable_Singleton
	{
	private:
		Mixin_Movable_Singleton() = default;

		LandscapePhysics* m_landPhys;

		friend class Mixin_Movable;
	public:
		Mixin_Movable_Singleton(Mixin_Movable_Singleton const&) = delete;
		Mixin_Movable_Singleton& operator= (Mixin_Movable_Singleton const&) = delete;

		static Mixin_Movable_Singleton& Instance()
		{
			static Mixin_Movable_Singleton s;
			return s;
		}

		static void SetLandscapePhysicsPtr(LandscapePhysics* PhysPtr);
	};
}