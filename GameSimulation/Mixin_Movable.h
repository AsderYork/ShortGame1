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

		btQuaternion m_orientation = btQuaternion(0.0f, 0.0f, 0.0f);
		btQuaternion m_rotation = btQuaternion(0.0f, 0.0f, 0.0f);
		/*Just like velocity is a rate at which position is changing(It better be called speed)
		rotation is a rate, at which entity changes it's orientation.
		*/

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
		void SetPosition(float x, float y, float z);
		void SetPosition(const btVector3 Pos);

		void SetVelocity(float X, float Y, float Z);
		void SetVelocity(const btVector3 Vel);
		inline btVector3 getVelocity() { return m_velocity; }
		inline float getSpeed() { return m_speed; }


		bool tick(const GameTime delta);

		inline const btVector3& getPos() {	return m_pos;	}

		/**!
		Orientation of the object is determied by 3 values(pitch, yaw, roll) between 0 and 360.
		Values outside of this range will be wraped.
		*/
		inline void setOrientation(float Pitch, float Yaw, float Roll) { m_orientation.setEuler(Yaw, Pitch, Roll); }
		inline void SetOrientation(btQuaternion orient) { m_orientation = orient;}

		inline void addOrientation(float Pitch, float Yaw, float Roll) { m_orientation *= btQuaternion(Yaw, Pitch, Roll); }
		inline void addOrientation(btQuaternion orient) { m_orientation *= orient; }

		inline btQuaternion getOrientation() { return m_orientation; }

		/**Set rotation speed per second
		*/
		inline void setRotation(float Pitch, float Yaw, float Roll) { m_rotation.setEuler(Yaw, Pitch, Roll); }
		inline void setRotation(btQuaternion rot) { m_rotation = rot; }

		/**Adds given rotation to an object 
		*/
		inline void addRotation(float Pitch, float Yaw, float Roll) { m_rotation *= btQuaternion(Yaw, Pitch, Roll); }
		inline void addRotation(btQuaternion rot) { m_rotation *= rot; }

		inline btQuaternion getRotation() { return m_rotation; }


		virtual bool NeedsUpdate() override;

		virtual void SendUpdate(cereal::BinaryOutputArchive & archive, const UpdateReason reason) override;

		virtual bool CheckAndReciveUpdate(cereal::BinaryInputArchive & archive, const GameTime UpdateLag) override;

		virtual void ApplyUpdate(cereal::BinaryInputArchive& archive) override;


	};
}