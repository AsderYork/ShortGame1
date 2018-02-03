#pragma once
#include <unordered_map>
#include <memory>
#include "GS_Entity_Base.h"

namespace GEM::GameSim
{
	using ENTITY_ID_TYPE = uint32_t;

	class EntityController
	{
	private:
		std::unordered_map<ENTITY_ID_TYPE, std::unique_ptr<Entity_Base>> m_entityMap;
	public:

		template<class T, typename... Args>
		typename std::enable_if<std::is_base_of_v<Entity_Base, T>, T*>::type
			AddNewEntity(ENTITY_ID_TYPE id, Args&&... args)
		{
			auto UniPtr = std::make_unique<T>(std::forward<Args>(args)...);
			auto Ptr = UniPtr.get();
			auto EmplaceResult = m_entityMap.emplace(id, std::move(UniPtr));
			return EmplaceResult.second ? Ptr : nullptr;
		}

		template<class T, typename... Args>
		T*	AddNewEntity(Args&&... args)
		{
			static_assert(false, "This method can be used only for classes, derived from GEM::GameSim::Entity_Base!");
			return nullptr;
		}
	};

}