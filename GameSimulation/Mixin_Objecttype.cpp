#include "Mixin_Objecttype.h"

#include <LogHelper.h>
#include <cereal\archives\binary.hpp>

namespace GEM::GameSim {
	void Mixin_Objecttype::SendUpdate(cereal::BinaryOutputArchive & archive, const UpdateReason reason)
	{
		archive(m_typeid);
	}
	bool Mixin_Objecttype::CheckAndReciveUpdate(cereal::BinaryInputArchive & archive, const GameTime UpdateLag)
	{
		LOGCATEGORY("Mixin_Objecttype/CheckAndReciveUpdate").alert("Someone is trying to update type of an entity!");
		return false;
	}

	void Mixin_Objecttype::ApplyUpdate(cereal::BinaryInputArchive & archive)
	{
		archive(m_typeid);
	}

}



