#pragma once
#include "EntitiesBase.h"
#include <map>
#include <functional>
#include <cereal\cereal.hpp>
#include <cereal\archives\binary.hpp>
#include <iostream>
#include <tuple>

#include "EntitiesBase.h"
#include "Mixin_Movable.h"

namespace GEM::GameSim
{

	
	
	

	class Mixin_Controller
	{
	private:
		/**!
		So every mixin class must register itself and all it's controlled methods here.
		With unique values for every class and for every method in class. Methods of different classes can share same id.
		*/
		struct ClassData
		{
			std::string name;
			struct MethodData
			{
				std::string name;
				std::function<void(EntityBase*, cereal::BinaryInputArchive  &)> method;

				MethodData(std::string n, std::function<void(EntityBase*, cereal::BinaryInputArchive  &)> m) : name(n), method(m) {}
			};
			std::map<int, MethodData> methods;

			ClassData(std::string n) : name(n) {}
		};
		std::map<int, ClassData> m_methods;



	public:
		static Mixin_Controller& Instance()
		{
			static Mixin_Controller s;
			return s;
		}

		/**!
		Registeres new mixin class. Return true, if registeres successfully.
		ID and name must be unique
		*/
		bool RegisterMixinClass(int classID, std::string className) {
			return m_methods.emplace(classID, className).second;
		}

		/**!
		Registeres new method of a mixin class. Return true if registered successfully.
		*/
		bool RegisterMethod(int classID, int MethodID, std::function<void(EntityBase*, cereal::BinaryInputArchive  &)> func, std::string name)
		{
			auto& Class = m_methods.find(classID);
			if (Class == m_methods.end()) { return false; }

			return Class->second.methods.emplace(MethodID, ClassData::MethodData(name, func)).second;
		}




	private:
		Mixin_Controller() {};
		~Mixin_Controller() {};
		Mixin_Controller(Mixin_Controller const&) = delete;
		Mixin_Controller& operator= (Mixin_Controller const&) = delete;
	};
		

	template<typename TFunc, typename TObj,typename Tpl, std::size_t... I>
	void BetterInvoke(TFunc func, TObj obj, Tpl& Tuple, std::index_sequence<I...>)
	{
		std::invoke(func, obj, std::get<I>(Tuple)...);
	}

	
	template < typename classname, typename ... funcargs, typename Indices = std::make_index_sequence<sizeof...(funcargs)>>
	std::function<void(EntityBase*, cereal::BinaryInputArchive &)> Grab(void (classname::*func)(funcargs...))
	{
		return std::function<void(EntityBase*, cereal::BinaryInputArchive &)>(
			[=](EntityBase* base, cereal::BinaryInputArchive & ar) {
			std::tuple<funcargs...> args;
			GEM::Helper::for_each(args, [&](int index, auto&& T) {ar(T); });


			auto Ptr = dynamic_cast<classname*>(base->GetMixinByID(classname::MixinID));
			BetterInvoke(func, Ptr, args, Indices{});
		}
		);
	}

	

	

	class CommandRetranslator
	{

	};

	
}