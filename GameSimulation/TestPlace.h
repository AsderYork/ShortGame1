#pragma once
#include "EntitiesBase.h"
#include <map>
#include <functional>
#include <cereal\cereal.hpp>
#include <cereal\archives\binary.hpp>
#include <iostream>
#include <tuple>

namespace GEM::GameSim
{

	class NewEntityBase
	{
	public:
		virtual bool tick(float delta) = 0;
		virtual ~NewEntityBase() {};
	};
	
	
	class Mixin_base
	{
	public:
		/**!
		Called every simulation tick. Must return true, if everything is ok, false will terminate simulation, probably
		*/
		virtual bool tick(float delta) = 0;
		virtual ~Mixin_base() {};
	};

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
				std::function<void(Mixin_base*, cereal::BinaryInputArchive &)> method;

				MethodData(std::string n, std::function<void(Mixin_base*, cereal::BinaryInputArchive &)> m) : name(n), method(m) {}
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
		bool RegisterMethod(int classID, int MethodID, std::function<void(Mixin_base*, cereal::BinaryInputArchive &)> func, std::string name)
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


	class Mixin_Movable : public Mixin_base
	{
	private:
		
	public:
		float x = 0, y = 0, z = 0;
		void Move(cereal::BinaryInputArchive & Arc)
		{
			Arc(x, y, z);
		}

		bool tick(float delta) { return true; }

	};

	
	/**!
	This meta-class should be used to represent all antities in the game!
	To add properties use various mixins.
	*/
	template<typename...Mixins>
	class MixedEntity : public NewEntityBase
	{
	private:
	public:

		std::tuple<Mixins...> m_mixins;
		/**!
		Initialize all mixins in an entity
		*/
		MixedEntity(Mixins&&...mixins)
		{			
			m_mixins = std::forward_as_tuple(mixins...);
		}

		template<class T>
		T& get()
		{
			return std::get<T>(m_mixins);
		}

		bool tick(float delta)
		{
			return (... && std::get<Mixins>(m_mixins).tick(delta));
		}
	};


	class CommandRetranslator
	{

	};

	
}