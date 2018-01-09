#include "EntitiesBase.h"
#include <cassert>
#ifdef ENTITY_BASE_ID_UNQIUE_PROOF
//If Unique heper is activated, this should be added to a constructor, to pass recived unique value
#define OPTIONAL_BASE_CONSTRUCTOR GEM::Helper::UniqueInstanceHelper<EntityBase, GS_Enitity_ID_Type>(id),
std::unordered_set<GEM::GameSim::GS_Enitity_ID_Type> GEM::Helper::UniqueInstanceHelper<GEM::GameSim::EntityBase, GEM::GameSim::GS_Enitity_ID_Type>::m_UnqiueProofSet;
#else
//Otherwise there must be just nothing
#define OPTIONAL_BASE_CONSTRUCTOR
#endif

GEM::GameSim::EntityBase::EntityBase(const GS_Enitity_ID_Type id) : OPTIONAL_BASE_CONSTRUCTOR m_id(id)
{
	assert(id != ENTITY_ID_NONE);//No entitiy can have this ID!
}
