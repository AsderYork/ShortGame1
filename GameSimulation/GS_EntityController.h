#pragma once
#include <unordered_map>
#include <memory>
#include "GS_Entity_Base.h"
#include <vector>

namespace GEM::GameSim
{
	using ENTITY_ID_TYPE = uint32_t;

	class EntityController
	{
	private:
		std::unordered_map<ENTITY_ID_TYPE, std::unique_ptr<Entity_Base>> m_entityMap;
	public:

		/**!
		Adds new entity of specified class.
		Only classes derived from GEM::GameSim::Entity_Base should be used to instanciate
		this template.
		\param[in] id ID of entity being created
		\param[in] args Pack of arguments to be forwarded to entity constructor
		\returns A pointer to a newly created entity in controller, or nullptr if entity wasn't added
		*/
		template<class T, typename... Args>
		T*	AddNewEntity(ENTITY_ID_TYPE id, Args&&... args)
		{
			auto UniPtr = std::make_unique<T>(std::forward<Args>(args)...);
			auto Ptr = UniPtr.get();
			auto EmplaceResult = m_entityMap.emplace(id, std::move(UniPtr));
			return EmplaceResult.second ? Ptr : nullptr;
		}

		/**!
		Remove entity with given id if there is one. Return true if entity is removed, false otherwise
		*/
		bool RemoveEntity(ENTITY_ID_TYPE id);

		/**!
		Returns a pointer to an entity with given id, if there is one. Otherwise nullptr will be returned.
		*/
		Entity_Base* GetEntity(ENTITY_ID_TYPE id);

		/**!
		Returns overall number of entities
		*/
		ENTITY_ID_TYPE GetEntitiesCount() const;

		

	};

}