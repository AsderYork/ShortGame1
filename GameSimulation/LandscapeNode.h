#pragma once
#include <cstdint>

namespace GEM::GameSim
{

	struct LandscapeNode
	{

		enum class SolidType : uint8_t
		{
			empty,

			//Soil
			Soil_rich,
			Soil_depleated,
			Soil_dry,
			Sand,
			Soil_normal,
			Soil_frozen,

			//ice
			Ice_clear,
			Ice_white,

			//Stright up rocks
			Rock_Andesite,
			Rock_Granite,
			Rock_Chalk,
			Rock_Limestone,
			Rock_Anthracite,

			//Metal ores
			MetalOre_Iron_ore,

			//Non-metallic ores
			Ore_Coal,
			Ore_Moonglow
		} Solid;
		uint8_t SolidSubtype;
		uint8_t SolidAmount;

		LandscapeNode() : Solid(SolidType::Soil_normal), SolidSubtype(0), SolidAmount(0) {}

		template<class Archive>
		void serialize(Archive & archive)
		{
			archive(Solid, SolidSubtype, SolidAmount);
		}
	};

}
