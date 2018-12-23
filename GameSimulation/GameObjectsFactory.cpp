#include "GameObjectsFactory.h"
#include <LogHelper.h>

#include "Object_Player.h"

#define REGISTER_OBJECT(objname) m_map.insert({Helper::Hasher<GAMEOBJECT_TYPE_ID>::Hash(#objname, "GameObjects"), [](ENTITY_ID_TYPE id) { \
auto newobj = std::make_unique<objname>(); \
newobj->m_id = id; \
static_cast<Mixin_Objecttype*>(newobj->GetMixinByID(Mixin_Objecttype::MixinID))->settypeid(Helper::Hasher<GAMEOBJECT_TYPE_ID>::Hash("objname", "GameObjectTypes")); \
return newobj; \
}})

namespace GEM::GameSim
{

	std::map<GAMEOBJECT_TYPE_ID, std::function<std::unique_ptr<EntityBase>(ENTITY_ID_TYPE)>> GameObjectFactory::m_map;


	void GameObjectFactory::RegisterAllObjects()
	{
		auto newobj = std::make_unique<Object_Player>();
		REGISTER_OBJECT(Object_Player);
	}



	std::unique_ptr<EntityBase> GameObjectFactory::CreateObject(GAMEOBJECT_TYPE_ID id, ENTITY_ID_TYPE entid)
	{
		auto builder = m_map.find(id);
		if (builder == m_map.end())
		{
			LOGCATEGORY("GameObjectFactory/CreateObject").error("Requested object [%i] is not registered", id);
			return nullptr;
		}
		else {
			return (builder->second)(entid);
		}
	}

	std::unique_ptr<EntityBase> GameObjectFactory::CreateObject(std::string ObjTypeName, ENTITY_ID_TYPE entid)
	{
		auto id = Helper::Hasher<GAMEOBJECT_TYPE_ID>::Hash(ObjTypeName, "GameObjectTypes");
		auto builder = m_map.find(id);
		if (builder == m_map.end())
		{
			LOGCATEGORY("GameObjectFactory/CreateObject").error("Requested object [%s] is not registered", ObjTypeName.c_str());
			return nullptr;
		}
		else {
			return (builder->second)(entid);
		}
	}


	std::optional<GAMEOBJECT_TYPE_ID> GameObjectFactory::getObjectType(EntityBase* obj) {

		auto objectMixin = obj->GetMixinByID(Mixin_Objecttype::MixinID);
		if (objectMixin != nullptr) {
			return static_cast<Mixin_Objecttype*>(objectMixin)->gettypeid();
		}

		return std::nullopt;
	}

}


