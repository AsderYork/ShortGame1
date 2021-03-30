#pragma once

#include <Bullet3Common/b3Vector3.h>

#include <vector>


namespace GEM::GameSim {

	/**
		Decides what chunks should be loaded and what should be unloaded
	*/
	class LandscapeVisibilityManager;

	

	class LandscapeVisibilityManager
	{
	public:
		using MapViewerId = int;

	private:

		class MapViewer
		{
		private:
			static MapViewerId idGenerator;
		public:
			MapViewerId Id;
			b3Vector3 Pos;
			float VisibilityRange;

			MapViewer(b3Vector3 pos, float visibilityRange) {
				Id = idGenerator++;
				Pos = pos;
				VisibilityRange = visibilityRange;
			}

			bool operator<(MapViewer const& right) const {
				return Id < right.Id;
			}


		};

		std::vector<MapViewer> Viewers;

	public:

		using MapViewerId = int;
		
		/**
		Crates new mapViewer. MapViewer describes a position, from which an entity views the world.
		\param[in] pos Starting position of the viewer
		\param[in] visibilityRange How far in units this entity should see
		*/
		MapViewerId AddMapViewer(b3Vector3 pos, float visibilityRange);

		/**
		Updates the position of mapViewer with given id.
		\warning. ID must be valid
		*/
		void UpdateMapViewer(MapViewerId id, b3Vector3 pos);
		
		void DeleteMapViewer(MapViewerId id);

		/**
		Calculates visible chunks and chunks that should be cleared for all exisitng Viewers;
		*/
		void Frame();
		

	};



}
