#pragma once
#include <cstdint>
#include "Helper_UniqueInstanceGuarantee.h"
#define ENTITY_BASE_ID_UNQIUE_PROOF
namespace GEM::GameSim
{
	//4 billions of entities!
	using GS_Enitity_ID_Type = uint32_t;
	/**
	Everything have a starting point. The entities is not en exclusion!
	This ID is reserved for errors. If an entity gets this ID' it's an error.
	*/
	const GS_Enitity_ID_Type ENTITY_ID_NONE = 0;


	/**!
	A base class for all entities.
	There is a lot of different things in a game, some big and some small,
	but you need to keep track of all of them.
	This class holds the most basic properties, shared among all of the entities, like ID's names e.t.c.
	But it's not resposible for providing uique ID's or enything like that
	*/
#ifdef ENTITY_BASE_ID_UNQIUE_PROOF
	class EntityBase : public GEM::Helper::UniqueInstanceHelper<EntityBase, GS_Enitity_ID_Type>
#else
	class EntityBase
#endif
	{
	protected:
		//Uniquely identifies an entity.
		const GS_Enitity_ID_Type m_id;
		
	public:
		/**!
		\param[in] id Unqiue identifier of an entitiy. Owner must guarante,
		that every existing instances of this class and it's derivatives have unique ID.
		*/
		EntityBase(const GS_Enitity_ID_Type id);

		/**!
		Entities need names. It is useful for debugging and even for in-game usage as in-game internal name;
		The resultig names follows the same rules as variable names in C++.
		Name must be unique for every derived class.
		*/
		virtual std::string getEntityClassName() const =0;

		virtual ~EntityBase() {}
	};

}