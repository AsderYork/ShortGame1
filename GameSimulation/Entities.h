#pragma once
#include <cstdint>

namespace GEM::GameSim
{

	/**!
	Some stuff about entities. We really need some default value for them. And it will be zero.
	No entity should use that value. NEVER!
	*/
	using GS_Enitity_ID_Type = uint32_t;
	const GS_Enitity_ID_Type ENTITY_ID_NONE = 0;

}