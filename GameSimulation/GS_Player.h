#pragma once
#include <cstdint>
#include <string>

namespace GEM::GameSim
{
		using GS_PlayerID_Type = uint32_t;		
		/**!
		Holds basic data about a player

		This should work like a ticket of a player data.
		It is recomended to perform search based.

		The way values of this type get compared can be controlled with GAMESIM_PLAYER_ONLY_ID_COMPARISON definition.
		If this flag is defined, comparisons will compare ID's which implies that name uniqueness is allready guaranteed.
		*/
		struct PlayerData
		{
			const GS_PlayerID_Type m_id;
			const std::string m_playerName;

			PlayerData(GS_PlayerID_Type id, std::string name) : m_id(id), m_playerName(name) {};
		};

		//Comparisons
		bool operator==(const PlayerData& lhs, const PlayerData& rhs)
		{
#ifdef GAMESIM_PLAYER_ONLY_ID_COMPARISON
			return (lhs.m_id == rhs.m_id)
#else
			return (lhs.m_id == rhs.m_id) && (lhs.m_playerName == rhs.m_playerName);
#endif
		}
		bool operator!=(const PlayerData& lhs, const PlayerData& rhs)
		{
			return !(lhs == rhs);
		}
		bool operator==(const PlayerData& lhs, const GS_PlayerID_Type& v)
		{
			return (lhs.m_id == v);
		}
		bool operator!=(const PlayerData& lhs, const GS_PlayerID_Type& v)
		{
			return (lhs != v);
		}
		bool operator==(const GS_PlayerID_Type& v, const PlayerData& rhs)
		{
			return (v == rhs.m_id);
		}
		bool operator!=(const GS_PlayerID_Type& v, const PlayerData& rhs)
		{
			return (v != rhs);
		}
		bool operator==(const PlayerData& lhs, const std::string& v)
		{
			return (lhs.m_playerName == v);
		}
		bool operator!=(const PlayerData& lhs, const std::string& v)
		{
			return (lhs != v);
		}
		bool operator==(const std::string& v, const PlayerData& rhs)
		{
			return (v == rhs.m_playerName);
		}
		bool operator!=(const std::string& v, const PlayerData& rhs)
		{
			return (v != rhs);
		}

}

/**
This part is used to specialize std::less for PlayerData type
*/
namespace std
{	
	template<> struct hash<GEM::GameSim::PlayerData>
	{
		std::size_t operator()(GEM::GameSim::PlayerData const& s) const noexcept
		{
			return static_cast<std::size_t>(s.m_id);
		}
	};
	
	/*
	template<> struct less<GEM::GameSim::PlayerData>
	{
		using is_transparent = void;
		bool operator()(GEM::GameSim::PlayerData const& lhs, GEM::GameSim::PlayerData const& rhs) const noexcept
		{
			return lhs.m_id < rhs.m_id;
		}
		bool operator()(GEM::GameSim::PlayerData const& lhs, GEM::GameSim::GS_PlayerID_Type const& rhs) const noexcept
		{
			return lhs.m_id < rhs;
		}
		bool operator()(GEM::GameSim::GS_PlayerID_Type const& lhs, GEM::GameSim::PlayerData const& rhs) const noexcept
		{
			return lhs < rhs.m_id;
		}


		bool operator()(GEM::GameSim::PlayerData const& lhs, std::string const& rhs) const noexcept
		{
			return lhs.m_playerName < rhs;
		}
		bool operator()(std::string const& lhs, GEM::GameSim::PlayerData const& rhs) const noexcept
		{
			return lhs < rhs.m_playerName;
		}

	};
	*/

}