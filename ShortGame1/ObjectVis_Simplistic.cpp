#include "stdafx.h"
#include "ObjectVis_Simplistic.h"


namespace GEM {


	ObjectVis_Simplistic::ObjectVis_Simplistic(GameSimController * gsController) 
	{
		gsController->m_entities.registerCreateListener([this](auto& val) {onEntitiyCreated(val); });
		gsController->m_entities.registerDestroyListener([this](auto& val) {onEntitiyDestroyed(val); });

	}

	ObjectVis_Simplistic::~ObjectVis_Simplistic()
	{
		m_createdConnection.disconnect();
		m_destroyedConnection.disconnect();
	}

}
