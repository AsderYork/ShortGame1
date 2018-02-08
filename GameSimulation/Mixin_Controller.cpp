#include "Mixin_Controller.h"

namespace GEM
{

	bool GameSim::Mixin_Controller::RegisterMixinClass(int classID, std::string className) {
		return m_methods.emplace(classID, className).second;
	}

	bool GameSim::Mixin_Controller::RegisterMethod(int classID, int MethodID, std::function<void(EntityBase*, cereal::BinaryInputArchive&)> func, std::string name)
	{
		auto& Class = m_methods.find(classID);
		if (Class == m_methods.end()) { return false; }

		return Class->second.methods.emplace(MethodID, ClassData::MethodData(name, func)).second;
	}

	bool GameSim::Mixin_Controller::ApplyCommand(EntityBase * Entity, MixinCommandRetranslator & Command)
	{
		auto CalledMixin = m_methods.find(Command.m_classID);
		if (CalledMixin == m_methods.end()) { return false; }

		auto CalledMethod = CalledMixin->second.methods.find(Command.m_methodID);
		if (CalledMethod == CalledMixin->second.methods.end()) { return false; }

		try
		{
			cereal::BinaryInputArchive  oarchive(Command.m_paramStream);
			CalledMethod->second.method(Entity, oarchive);
		}
		catch (const std::exception&)
		{
			return false;
		}
		return true;
	}

}