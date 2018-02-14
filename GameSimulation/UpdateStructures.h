#pragma once
#include "Mixin_Base.h"
#include "GS_EntityController.h"

#include <cereal\cereal.hpp>
#include <vector>
#include <variant>


namespace GEM::GameSim
{
	/**!
	Result of a regualar entity update.
	*/
	struct EntityRegularUpdate
	{
		std::vector<std::pair<MIXIN_ID_TYPE, std::string>> PerMixinUpdates;

		template<class Archive>
		void serialize(Archive & archive)
		{
			archive(PerMixinUpdates);
		}
	};

	/**!
	Result of a appearing entity update.
	*/
	struct EntityAppearingUpdate
	{
		std::vector<MIXIN_ID_TYPE> EntityMixins;
		std::vector<std::pair<MIXIN_ID_TYPE, std::string>> PerMixinUpdates;

		template<class Archive>
		void serialize(Archive & archive)
		{
			archive(EntityMixins, PerMixinUpdates);
		}
	};

	struct UpdateData
	{
		ENTITY_ID_TYPE EntityID;
		std::variant<EntityRegularUpdate, EntityAppearingUpdate> Data;

		UpdateData(EntityRegularUpdate&& Regular, ENTITY_ID_TYPE id) : Data(Regular), EntityID(id){}
		UpdateData(EntityAppearingUpdate&& Appearing, ENTITY_ID_TYPE id) : Data(Appearing), EntityID(id) {}
		UpdateData() {}

		template<class Archive>
		void save(Archive & archive) const
		{
			if (std::holds_alternative<EntityRegularUpdate>(Data))
			{
				archive(true, EntityID, std::get<EntityRegularUpdate>(Data));
			}
			else
			{
				archive(false, EntityID, std::get<EntityAppearingUpdate>(Data));
			}
		}

		template<class Archive>
		void load(Archive & archive)
		{
			bool UpdateType = false;
			archive(UpdateType);
			if (UpdateType)
			{
				archive(EntityID);
				EntityRegularUpdate erg;
				archive(erg);
				Data = erg;
			}
			else
			{
				archive(EntityID);
				EntityAppearingUpdate eap;
				archive(eap);
				Data = eap;
			}
		}
	};

	/**!
	So, updates. Imagine 100 player playing. Some of them will recive nearly identical updatesm
	some will not.
	So there was 3 ideas.
	1) Make a separate storage for updates on a per player basis. This means that there is as much duplicates as possible!
	2) All immovable objects have one storage, movvables stored on a per-player basis. Well, it won't work. Because updates
	may differ even for immovables! New player must recive APPEAR packets, while players that allready have this entity should only
	recive REGULAR.
	3)Make a single storage for all updates and with every update, store, which players should recive it! In that case there will be no duplicates.
	If noone needs some particular update, it just won't make it to this storage, so there is absolutely no faffing about!

	And some times have passed, some more stuff were fleshed and it is clear, that no matter what, right before sending we will
	have option-1-like situaltion. So why even bother with option 3?

	We will have a map with values per player. Right at the moment of building. And then we just send it as is.
	*/
}