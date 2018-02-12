#pragma once
#include "Mixin_Base.h"

namespace GEM::GameSim
{

	/**!
	Provides property of health to an entity
	*/
	class Mixin_Health : public Mixin_base
	{
	private:

	public:
		static const int MixinID = 12;

		
		float m_health;
		float m_maxHealth;

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




		bool tick(float delta) { return true; }

	};
}