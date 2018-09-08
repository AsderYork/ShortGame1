#pragma once

#include "EntitiesBase.h"

#include <map>
#include <functional>
#include <cereal\cereal.hpp>
#include <cereal\archives\binary.hpp>
#include <sstream>


#define REGISTER_MIXIN_CLASS(classname) Mixin_Controller::Instance().RegisterMixinClass(classname::MixinID, #classname);
#define REGISTER_MIXIN_METHOD(classname, methodname, id) Mixin_Controller::Instance().RegisterMethod(classname::MixinID, id, Mixin_Controller::Grab(&classname::methodname), #methodname);


	namespace GEM::GameSim
	{

		/**!
		Holds command call for a mixins
		*/
		class MixinCommandRetranslator
		{
			int m_classID;
			int m_methodID;
			std::stringstream m_paramStream;

			friend class Mixin_Controller;
			MixinCommandRetranslator(const MixinCommandRetranslator&) = delete;

		public:

			MixinCommandRetranslator(MixinCommandRetranslator&&) = default;
			/**!
			/param[in] classID ID of a mixin, that should recive this call
			/param[in] MethodID ID of a method in a class, that should be called
			/param[in] args... Arguments, that should be passed. Arguments must match with arguments of a called method exactly!
			*/
			template<typename... Args>
			MixinCommandRetranslator(int classID, int MethodID, Args...args) : m_classID(classID), m_methodID(MethodID)
			{
				cereal::BinaryOutputArchive oarchive(m_paramStream);
				oarchive(args...);
			}
		};


		/**!
		Singleton. Controlls all mixins and allow remote call of some of their methods
		*/
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

			template<typename TFunc, typename TObj, typename Tpl, std::size_t... I>
			static void BetterInvoke(TFunc func, TObj obj, Tpl& Tuple, std::index_sequence<I...>)
			{
				std::invoke(func, obj, std::get<I>(Tuple)...);
			}



		public:
			/**!
			Lazy initialization.
			/returns Returns a reference to a mixin controller singleton
			*/
			static Mixin_Controller& Instance()
			{
				static Mixin_Controller s;
				return s;
			}

			/**!
			Registeres new mixin class. Return true, if registeres successfully.
			ID and name must be unique.
			*/
			bool RegisterMixinClass(int classID, std::string className);

			/**!
			Registeres new method of a mixin class. Return true if registered successfully.
			/param[in] ClassID ID of a class, that own this method. Must be unique
			/param[in] MethodID ID that will be assigned to this method. Must be unique for a given class
			/param[in] func A function, that should be able to retranslate parameteres to a function being registered
			/param[in] name Name that will be assigned to this method
			/returns Returns true if method registered successfully, false otherwise

			/note All parameters of a method registered MUST be default-constructable!
			And return value MUST be void.
			*/
			bool RegisterMethod(int classID, int MethodID, std::function<void(EntityBase*, cereal::BinaryInputArchive  &)> func, std::string name);

			/**!
			Transforms a mixin method to a functional object, that can be registered
			/param[in] func A function of a mixin, that should be enveloped in a functional object
			/returns Returns a functional object that is ready for registration
			*/
			template < typename classname, typename ... funcargs, typename Indices = std::make_index_sequence<sizeof...(funcargs)>>
			static std::function<void(EntityBase*, cereal::BinaryInputArchive &)> Grab(void (classname::*func)(funcargs...))
			{
				return std::function<void(EntityBase*, cereal::BinaryInputArchive &)>(
					[=](EntityBase* base, cereal::BinaryInputArchive & ar) {
					std::tuple<funcargs...> args;
					Helper::for_each(args, [&](int index, auto&& T) {ar(T); });


					auto Ptr = dynamic_cast<classname*>(base->GetMixinByID(classname::MixinID));
					BetterInvoke(func, Ptr, args, Indices{});
				}
				);
			}

			/**!
			Applies a command to an entity
			\returns Return true, if command was applied successfully, false otherwise
			*/
			bool ApplyCommand(EntityBase* Entity, MixinCommandRetranslator& Command);


		private:
			Mixin_Controller() {};
			~Mixin_Controller() {};
			Mixin_Controller(Mixin_Controller const&) = delete;
			Mixin_Controller& operator= (Mixin_Controller const&) = delete;
		};
	}
