#pragma once
#include "Mixin_Base.h"
#include "EntitiesBase.h"
#include <vector>
#include <memory>

#include "Mixin_Movable.h"


namespace GEM::GameSim
{
	class StaticlyMixedEntity : public EntityBase
	{
	private:
		const std::vector<std::unique_ptr<Mixin_base>> m_mixins;
	public:
		StaticlyMixedEntity(std::vector<std::unique_ptr<Mixin_base>>&& vec) : m_mixins(std::move(vec)) {}

		virtual Mixin_base * GetMixinByID(int i) override;
		virtual bool tick(float delta) override;


		virtual std::vector<Mixin_base*> getAllMixins() override;
	};

	template<typename...TMixins>
	class EntityGenerator
	{
		template<typename... Args>
		struct Impl;

		template <typename First, typename... Args>
		struct Impl<First, Args...>
		{
			static std::unique_ptr<Mixin_base> GenerateMixinIfIDIsCorrenct(MIXIN_ID_TYPE id)
			{

				if (First::MixinID == id)
				{
					return std::make_unique<First>();
				}

				return Impl<Args...>::GenerateMixinIfIDIsCorrenct(id);

			}
		};

		template <>
		struct Impl<>
		{
			static std::unique_ptr<Mixin_base> GenerateMixinIfIDIsCorrenct(MIXIN_ID_TYPE id)
			{
				return nullptr;
			}
		};
		
		
	public:

		std::unique_ptr<EntityBase> GenerateEntity(const std::vector<MIXIN_ID_TYPE> Mixins)
		{
			std::vector<std::unique_ptr<Mixin_base>> vec;

			for(auto& id : Mixins)
			{
				auto Ptr = Impl<TMixins...>::GenerateMixinIfIDIsCorrenct(id);
				if (Ptr == nullptr) { throw std::exception("Can't build an entity. Requested Mixin is unknown!"); }
				vec.emplace_back(std::move(Ptr));
			}

			return std::make_unique<StaticlyMixedEntity>(std::move(vec));
		}

	};
}