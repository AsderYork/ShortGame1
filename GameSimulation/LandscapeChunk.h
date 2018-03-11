#pragma once
#include <cstdint>
#include <cereal\cereal.hpp>


namespace GEM::GameSim
{
	constexpr uint32_t LandscapeChunk_Size = 16;
	constexpr uint32_t LandscapeChunk_Height = 64;

	struct LandscapeChunk_NodeData
	{
		//Values
		uint8_t Value;


		LandscapeChunk_NodeData(uint8_t Val) : Value(Val) {}
		LandscapeChunk_NodeData() : Value(0) {}
		/**!
		The contains of a chunk node is unknown for the time of writing so this method abstract the "fullness" of a node

		For empty node, 0 must be return. For completely full node, 1.0f should be returned and every intermediate amount of stuff
		in a node must result in some number in (0.0f, 1.0f) so that the more stuff there is in a node, the bigger the resulting value
		*/
		float getOverallAmount() const { return Value; }

		template<class Archive>
		void serialize(Archive & archive)
		{
			archive(Value);
		}
	};

	class LandscapeChunk
	{
	public:
		LandscapeChunk_NodeData Nodes[LandscapeChunk_Size][LandscapeChunk_Size][LandscapeChunk_Height];

		/**!
		Chunks can be changed. This variable is used to track the version of a chunk. Implementation must guarantee, that
		if to chunks have same position and version, they are identical.
		*/
		uint64_t Version;
		uint32_t PosX, PosY;

		template<class Archive>
		void serialize(Archive & archive)
		{
			archive(Version, PosX, PosY, Nodes);
		}
		
	};

}