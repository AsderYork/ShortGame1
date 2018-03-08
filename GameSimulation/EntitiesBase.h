#pragma once
#include "Mixin_Base.h"
#include "GameTime.h"

#include "Helper_TupleForeach.h"

namespace GEM::GameSim
{
	/**!
	Base class for all entities in simulation.
	It's purpose is to provide shared base for all instancess of MixedEntity metaclass
	and to allow access to a Mixins in an entity
	*/
	class EntityBase
	{
	public:		
		virtual Mixin_base* GetMixinByID(int i) const = 0;
		virtual std::vector<Mixin_base*> const getAllMixins() const = 0;


		virtual bool tick(const GameTime delta) = 0;
		virtual ~EntityBase() {};
	};

	/**!
	This meta-class should be used to represent all entities in the game!
	To add properties use various mixins.
	*/
	template<typename...Mixins>
	class MixedEntity : public EntityBase
	{
	private:

	public:

		std::tuple<Mixins...> m_mixins;
		/**!
		Initialize all mixins in an entity
		*/
		MixedEntity(Mixins&&... mixins) : m_mixins(std::move(mixins)...)
		{}

		/**!
		Return mixin of certain type
		*/
		template<class T>
		T& get()
		{
			return std::get<T>(m_mixins);
		}

		bool tick(GameTime delta)
		{
			return (... && std::get<Mixins>(m_mixins).tick(delta));
			return true;
		}

		/**!
		Returns a pointer to a mixin with given ID. If there is no such mixin, nullptr will be returned
		*/
		Mixin_base* GetMixinByID(int id) const
		{
			Mixin_base* retptr;
			GEM::Helper::for_each(m_mixins, [&](int index, auto&& T) {
				if (T.MixinID == id) { retptr = &T; }
			});
			return retptr;
		}

		virtual std::vector<Mixin_base*> const getAllMixins() const override
		{
			return std::vector<Mixin_base*>{&(std::get<Mixins>(m_mixins))...};
		}
	};


}