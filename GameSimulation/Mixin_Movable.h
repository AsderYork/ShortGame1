#pragma once
#include "Mixin_Base.h"
//#include <vmmlib\vector.hpp>
//#include <vmmlib/math.hpp>

#include <LinearMath\btVector3.h>
#include <chrono>

namespace GEM::GameSim
{
	/**!
	Allow entity to be moved around
	*/
	class Mixin_Movable : public Mixin_base
	{
	private:

	protected:
		btVector3 m_pos = btVector3(0, 0, 0);
		btVector3 m_velocity = btVector3(0, 0, 0);
		//Remembers when was the last time of gathering update from this mixin. If it was too long, it will start asking for an update
		std::chrono::system_clock::time_point m_lastUpdate;

		bool m_keepUpdating = true;
		float m_speed;

	public:

	 MIXIN_ID(43)

		 Mixin_Movable() : m_lastUpdate(std::chrono::system_clock::now()), m_speed(1.0f) {}

		/**!
		Shifts position of an entity
		*/
		void Shift(float X, float Y, float Z)
		{
			m_pos += btVector3(X, Y, Z);
		}

		void SetVelocity(float X, float Y, float Z);

		bool tick(const GameTime delta);

		inline const btVector3& getPos() {	return m_pos;	}

		virtual bool NeedsUpdate() override;

		virtual void SendUpdate(cereal::BinaryOutputArchive & archive, const UpdateReason reason) override;

		virtual void ReciveUpdate(cereal::BinaryInputArchive & archive, const GameTime UpdateLag) override;

		virtual void ApplyEvent(cereal::BinaryInputArchive& archive) override;

		virtual void ReciveEvent(const EventBase * const Event) override;

	};
}