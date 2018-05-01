#pragma once
#include "Mixin_Base.h"
#include "Mixin_Movable Singleton.h"
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

		struct CurrentMovement
		{
			enum class NormalMovement:char{FORWARD,BACKWARD,NONE} Normal;
			enum class StrafeMovement:char{LEFT,RIGHT,NONE} Strafe;
			
			CurrentMovement();
			btVector3 GetNewVelociyVectorBasedOnCurrentState();
		};
		CurrentMovement m_movementState;

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
		void SetPosition(float x, float y, float z);
		void SetPositionV(const btVector3 Pos);

		void SetVelocity(float X, float Y, float Z);
		void SetVelocityV(const btVector3 Vel);

		bool tick(const GameTime delta);

		inline const btVector3& getPos() {	return m_pos;	}

		virtual bool NeedsUpdate() override;

		virtual void SendUpdate(cereal::BinaryOutputArchive & archive, const UpdateReason reason) override;

		virtual bool CheckAndReciveUpdate(cereal::BinaryInputArchive & archive, const GameTime UpdateLag) override;

		virtual void ApplyEvent(cereal::BinaryInputArchive& archive) override;

		virtual void ReciveEvent(const EventBase * const Event) override;

	};
}