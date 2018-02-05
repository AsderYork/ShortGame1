#pragma once
#include "EntitiesBase.h"
#include <map>
#include <functional>
#include <cereal\cereal.hpp>
#include <cereal\archives\binary.hpp>
#include <iostream>

namespace GEM::GameSim
{
	class ParamPack
	{
	public:
		ParamPack() {};
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
				std::function<void(cereal::BinaryInputArchive &)> method;

				MethodData(std::string n, std::function<void(cereal::BinaryInputArchive &)> m) : name(n), method(m) {}
			};
			std::map<int, MethodData> methods;

			ClassData(std::string n) : name(n){}
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
		bool RegisterClass(int classID, std::string className) {
			return m_methods.emplace(classID, className).second;
		}

		/**!
		Registeres new method of a mixin class. Return true if registered successfully.
		*/
		bool RegisterMethod(int classID, int MethodID, std::function<void(cereal::BinaryInputArchive &)> func, std::string name)
		{
			auto& Class = m_methods.find(classID);
			if (Class == m_methods.end()) { return false; }
			
			return Class->second.methods.emplace(MethodID, name, func).second;			
		}






	private:
		Mixin_Controller() {};
		~Mixin_Controller() {};
		Mixin_Controller(Mixin_Controller const&) = delete;
		Mixin_Controller& operator= (Mixin_Controller const&) = delete;
	};

	class Mixin_base
	{
	};



	class Mixin_Movable : public Mixin_base
	{
	private:
		float x = 0, y = 0, z = 0;
	public:
		
		void Move(cereal::BinaryInputArchive & Arc)
		{
			Arc(x, y, z);
		}

	};


	class EntityMovable : public EntityBase, public Mixin_Movable
	{
	};


	
}