#pragma once
#include "Mixin_Base.h"
#include <chrono>

namespace GEM::GameSim
{

	/**!
	Provides property of health to an entity
	*/
	class Mixin_Health : public Mixin_base
	{
	private:
		float m_health;
		float m_maxHealth;
		bool isChanged = false;

		//Remembers when was the last time of gathering update from this mixin. If it was too long, it will start asking for an update
		std::chrono::system_clock::time_point m_lastUpdate;
	public:

		MIXIN_ID(12)

		inline decltype(m_health) getHealth() { return m_health; }
		inline decltype(m_maxHealth) getMaxHealth() { return m_maxHealth; }

		/**!
		If MaxHealth is set to 0, health will not have an upper limit
		*/
		Mixin_Health(float MaxHealth, float tmpHealth) : m_maxHealth(MaxHealth), m_health(tmpHealth)
		{}
		/**!
		TmpHealth will be set equal to MaxHealth
		If MaxHealth is set to 0, health will not have an upper limit.
		*/
		Mixin_Health(float MaxHealth) : m_maxHealth(MaxHealth), m_health(MaxHealth)
		{}

		Mixin_Health() : m_maxHealth(100), m_health(100)
		{}

		Mixin_Health(Mixin_Health &&) = default;

		/**!
		Set health between 0 and MaxHeakth if it's non-zero.
		Otherwise health have no upper limit
		*/
		void SetHealth(float Health)
		{
			isChanged = m_health != Health;
			m_health = Health;
			if (m_health < 0) { m_health = 0; }
			if (m_maxHealth != 0)
			{
				if (m_health > m_maxHealth)
				{
					m_health = m_maxHealth;
				}
			}
		}

		inline void changeHealth(float Delta) {
			isChanged = Delta != 0;
			m_health += Delta;
			if (m_health < 0)
			{
				m_health = 0;
			}
			else if (m_health > m_maxHealth)
			{
				m_health = m_maxHealth;
			}
		}


		virtual bool NeedsUpdate() override;


		bool tick(const GameTime delta) { return true; }


		virtual void SendUpdate(cereal::BinaryOutputArchive & archive, const UpdateReason reason) override;

		virtual bool CheckAndReciveUpdate(cereal::BinaryInputArchive & archive, const GameTime LagTime) override;

		virtual void ApplyUpdate(cereal::BinaryInputArchive& archive) override;

	};
}