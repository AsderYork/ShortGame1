#pragma once
#include "GameSimService.h"

#include <memory>
#include <map>

namespace GEM
{

	class ObjectVis_Simplistic {
	private:
		boost::signals2::connection m_createdConnection;
		boost::signals2::connection m_destroyedConnection;

	public:

		ObjectVis_Simplistic(GameSimController * gsController);
		~ObjectVis_Simplistic();


		void onEntitiyCreated(std::shared_ptr<GameSim::EntityBase> ent) {
			LOGCATEGORY("ObjectVis_Simplistic/onEntitiyCreated").info("Something was created!");
		}

		void onEntitiyDestroyed(std::shared_ptr<GameSim::EntityBase> ent) {
			LOGCATEGORY("ObjectVis_Simplistic/onEntitiyCreated").info("Something was destroyed!");
		}

	};

}