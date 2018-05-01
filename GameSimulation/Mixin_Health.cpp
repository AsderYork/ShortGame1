#include "Mixin_Health.h"

void GEM::GameSim::Mixin_Health::SendUpdate(cereal::BinaryOutputArchive & archive, const UpdateReason reason)
{
}

bool GEM::GameSim::Mixin_Health::CheckAndReciveUpdate(cereal::BinaryInputArchive & archive, const GameTime updateTime)
{
	return true;
}

void GEM::GameSim::Mixin_Health::ApplyEvent(cereal::BinaryInputArchive& archive)
{

}