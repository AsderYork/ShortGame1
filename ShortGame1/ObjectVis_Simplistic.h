#pragma once
#include <map>
#include <functional>

namespace GEM
{
	/**
	Describes a visualization of an object type
	*/
	class GameObjectModel
	{
	public:
		/**
		\brief Creates an object's visualization 
		*/
		virtual bool Init()=0;

		/**
		\brief updates object's visual according to game state changes
		*/
		virtual bool Frame()=0;
	};

	/**
	\brief Provides simplistic visualization of GameSim entities with respect to Object Model
	*/
	class ObjectVis_Simplistic {

	public:

		void Frame();

	};
}