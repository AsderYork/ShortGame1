#pragma once
#include "EntitiesBase.h"
#include "EntityIDType.h"
#include <unordered_map>
#include <memory>
#include <vector>
#include <optional>
#include <functional>
#include <boost/signals2.hpp>
#include <LinearMath/btVector3.h>

namespace GEM::GameSim
{

	class EntityController
	{
	private:
		std::unordered_map<ENTITY_ID_TYPE, std::shared_ptr<EntityBase>> m_entityMap;
		boost::signals2::signal<void(std::shared_ptr<EntityBase>)> m_createListeners;
		boost::signals2::signal<void(std::shared_ptr<EntityBase>)> m_destroyListeners;
	public:
		~EntityController();

		//*Signals

		inline auto registerCreateListener(std::function<void(std::shared_ptr<EntityBase>)> callback) {
			return m_createListeners.connect(callback);
		}

		inline auto registerDestroyListener(std::function<void(std::shared_ptr<EntityBase>)> callback) {
			return m_destroyListeners.connect(callback);
		}


		//Entity-addition/removal

		/**!
		Adds new entity of specified class.
		Only classes derived from GEM::GameSim::Entity_Base should be used to instanciate
		this template.
		\param[in] id ID of entity being created
		\param[in] args Pack of arguments to be forwarded to entity constructor
		\returns A pointer to a newly created entity in controller, or nullptr if entity wasn't added
		*/
		template<class T, typename... Args>
		std::weak_ptr<T>	AddNewEntity(ENTITY_ID_TYPE id, Args&&... args)
		{
			auto UniPtr = std::make_unique<T>(std::forward<Args>(args)...);
			auto Ptr = UniPtr.get();
			auto EmplaceResult = m_entityMap.emplace(id, std::move(UniPtr));
			if (EmplaceResult.second)
			{
				m_createListeners(UniPtr);
				return Ptr;
			}
			return nullptr;
		}

		template<class...TMixins>
		std::weak_ptr<MixedEntity<TMixins...>> MixNewEntity(ENTITY_ID_TYPE id, TMixins&&... constructors)
		{
			auto UniPtr = std::make_shared<MixedEntity<TMixins...>>(std::forward<TMixins>(constructors)...);
			auto Ptr = UniPtr.get();
			auto EmplaceResult = m_entityMap.emplace(id, std::move(UniPtr));
			if (EmplaceResult.second)
			{
				m_createListeners(UniPtr);
				return Ptr;
			}
			return nullptr;

		}

		inline std::weak_ptr<EntityBase> AddFreeEntity(ENTITY_ID_TYPE id, std::unique_ptr<EntityBase>&& ptr) {
			auto iter = m_entityMap.emplace(id, std::move(ptr));
			if (!iter.second) { return std::weak_ptr<EntityBase>(); }
			else 
			{
				m_createListeners(iter.first->second);
				return iter.first->second;
			}
		}

		/**!
		Returns a pointer to an entity with given id, if there is one. Otherwise nullptr will be returned.
		*/
		std::weak_ptr<EntityBase> GetEntity(ENTITY_ID_TYPE id);

		/**!
		Returns overall number of entities
		*/
		ENTITY_ID_TYPE GetEntitiesCount() const;

		/**!
		Remove entity with given id if there is one. Return true if entity is removed, false otherwise
		*/
		bool RemoveEntity(ENTITY_ID_TYPE id);

		//Exposed iterators

		inline auto begin() { return m_entityMap.begin(); }
		inline auto end() { return m_entityMap.end(); }

		std::list<std::weak_ptr<EntityBase>> getAllEntitiesInASphere(btVector3 center, float Radius);

		/**!
		This structure is used to hide the iterator of actual hash map
		*/
		class EntityListIterator
		{
			friend class EntityController;
			std::unordered_map<ENTITY_ID_TYPE, std::shared_ptr<EntityBase>>::iterator m_iter;
		public:
			EntityListIterator(EntityController* Controller) : m_iter(Controller->m_entityMap.begin()) {}

		};
		friend class EntityListIterator;

		/**!
		Allows iteration over entities.
		When this method is called with default parameter, it will return first entity, if there is one and produce a valid interator
		on the next entity. This iterator can be used in next call of this function, to retrieve another entity.

		When last entity is retrieved, all calls after it will not produce values. Iterator will not be destroyed.
		It is recomended to just let it fall off the stack when all iteration buisness is done.
		*/
		std::optional<std::pair<ENTITY_ID_TYPE, EntityBase*>> IterateOverEntities(std::unique_ptr<EntityListIterator> &&Iterator = std::unique_ptr<EntityListIterator>(nullptr))
		{
			if (Iterator == nullptr)
			{
				Iterator = std::make_unique<EntityListIterator>(this);
			}

			if (Iterator->m_iter != m_entityMap.end())
			{
				auto RetVal = std::make_pair(Iterator->m_iter->first, Iterator->m_iter->second.get());
				Iterator->m_iter++;
				return std::optional<std::pair<ENTITY_ID_TYPE, EntityBase*>>(RetVal);
			}
			return std::nullopt;
		}
	};

}