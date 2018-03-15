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

		For empty node, 0 must be return. For completely full node, 255 should be returned and every intermediate amount of stuff
		in a node must result in some number in (0, 255) so that the more stuff there is in a node, the bigger the resulting value
		*/
		uint8_t getOverallAmount() const { return Value; }

		template<class Archive>
		void serialize(Archive & archive)
		{
			archive(Value);
		}
	};

	class LandscapeChunk
	{
		LandscapeChunk_NodeData m_nodes[LandscapeChunk_Size][LandscapeChunk_Size][LandscapeChunk_Height];


		/**!
		Chunks can be changed. This variable is used to track the version of a chunk. Implementation must guarantee, that
		if to chunks have same position and version, they are identical.
		*/
		uint64_t m_version;
		int32_t m_posX, m_posZ;
	public:

		LandscapeChunk(int32_t x, int32_t z, uint64_t Version=0) :
			m_posX(x), m_posZ(z), m_version(Version) {}


		LandscapeChunk() :	m_posX(0), m_posZ(0), m_version(0) {}

		inline uint8_t getNodeValue (uint32_t x, uint32_t y, uint32_t z) const
		{return m_nodes[x][z][y].getOverallAmount();}

		inline LandscapeChunk_NodeData& getNode(uint32_t x, uint32_t y, uint32_t z)
		{
			return m_nodes[x][z][y];
		}

		inline uint64_t getVersion() const{ return m_version; }
		inline std::pair<int32_t, int32_t> getPosition() const{ return std::make_pair(m_posX, m_posZ); }

		template<class Archive>
		void serialize(Archive & archive)
		{
			archive(m_version, m_posX, m_posZ, m_nodes);
		}
		
	};

}