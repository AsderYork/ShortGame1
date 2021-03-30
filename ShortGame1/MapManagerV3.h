#pragma once

namespace GEM {

	/*! \page MapManager3 A new new map manager!
	So the map manager once again. Last time the problem was with TransVoxel implementation as it couldn't do multiple materials properly
	and it also couldn't do LOD. I fact the whole system whern't designed for multiple detail levels. There was a 1 to 1 relation between
	a map chunk, all it's additional data and a rendered chunk. Another uneccesery restriction was that a map wasn't devided verticaly
	so the only way to increase the map size was to change the sizes of individual chunks. It also made chunks much bigger.
	To fix this problem several design desicions have to be reconsidered.

	The frirst big question is whether MapManager should controll everything there is on the map or only the chunks? Well it looks
	like there's actualy should be two separate entities. The Word Manager should be the central hub for everything (Real objects with position,
	sky, and of course a land). But wait, isn't it's excatly what a GameSimulation does? Well yes, but it's kind of all other the place. So it
	is definetly should also be reworked in a near future.

	The second question is about LOD and granularity of simulation. Does LOD should affect only the visible things, or should it also affect
	physical representation? Bouth of these options have some pretty bad drawbacks. The variant with LOD affecting the physical simulation
	would mean that simulation would be very inacurate. It also would require a separate implementation for server(Whych would've been done anyways
	becouse server doesn't need graphics). The variant with LOD not affecting physical appearance of a landscape would require additional resources
	for generating and storing maximum LOD meshes for chunks. It also would lead to some visible anomalies when a less detailed variant of terrain
	differences severe enough from it's physical representation. But it's still would be a much better way to handle things 
	*/

	class MapManagerV3
	{
	private:



	public:
		MapManagerV3();

		

	};
	
}