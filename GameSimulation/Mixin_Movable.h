#pragma once
#include "Mixin_Base.h"
#include <vmmlib\vector.hpp>
#include <vmmlib/math.hpp>

namespace GEM::GameSim
{
	/**!
	Allow entity to be moved around
	*/
	class Mixin_Movable : public Mixin_base
	{
	private:

	protected:
		vmml::vec3f m_pos = vmml::vec3f(0, 0, 0);
		vmml::vec3f m_velocity = vmml::vec3f(0, 0, 0);


		bool m_keepUpdating = true;

	public:

	 MIXIN_ID(43)


		/**!
		Shifts position of an entity
		*/
		void Shift(float X, float Y, float Z)
		{
			m_pos += vmml::vec3f(X, Y, Z);
		}

		void SetVelocity(float X, float Y, float Z);

		bool tick(float delta);

		inline vmml::vec3f getPos() {	return m_pos;	}

		virtual bool NeedsUpdate() override;

		virtual void SendUpdate(cereal::BinaryOutputArchive & archive, const UpdateReason reason) override;

		virtual void ReciveUpdate(cereal::BinaryInputArchive & archive) override;

		virtual void ReciveEvent(const EventBase * const Event) override;

	};
}