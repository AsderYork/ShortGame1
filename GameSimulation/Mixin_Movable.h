#pragma once
#include "Mixin_Base.h"
namespace GEM::GameSim
{
	/**!
	Allow entity to be moved around
	*/
	class Mixin_Movable : public Mixin_base
	{
	private:
		float x = 0, y = 0, z = 0;
	public:

		static const int MixinID = 43;


		/**!
		Shifts position of an entity
		*/
		void Shift(float X, float Y, float Z)
		{
			x += X;
			y += Y;
			z += Z;
		}

		bool tick(float delta) { return true; }

	};
}