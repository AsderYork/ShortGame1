#pragma once
#include "GameObject.h"

#include <Hasher.h>
#include <map>
#include <functional>
#include <memory>
#include <optional>

namespace GEM::GameSim
{
	/**
		\brief Registers object types, allows simple creation of said object
	*/
	class GameObjectFactory
	{
	private:
		
		static std::map<GAMEOBJECT_TYPE_ID, std::function<std::unique_ptr<EntityBase>(ENTITY_ID_TYPE)>> m_map;
		


	public:

		static void RegisterAllObjects();
		/**
		\brief Creates an object with a given id.
		*/
		static std::unique_ptr<EntityBase> CreateObject(GAMEOBJECT_TYPE_ID id, ENTITY_ID_TYPE entid);


		static std::unique_ptr<EntityBase> CreateObject(std::string ObjTypeName, ENTITY_ID_TYPE entid);
		
		/**
		\brief Returns object's id, if this entity is really an object
		*/
		static std::optional<GAMEOBJECT_TYPE_ID> getObjectType(EntityBase* obj);
		
	};
	
}