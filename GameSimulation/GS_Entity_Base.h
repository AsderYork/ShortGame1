#pragma once
namespace GEM::GameSim
{
	/**!
	Base class for all entities.
	*/
	class Entity_Base
	{
	protected:
		/**!
		Every class of entities must have unique value
		*/
		const uint16_t m_EntityClassType;

	public:
		Entity_Base(uint16_t EntityClassType) : m_EntityClassType(EntityClassType) {};
		virtual ~Entity_Base() {};

		inline const uint16_t getEntityClass() const { return m_EntityClassType; }
	};
}