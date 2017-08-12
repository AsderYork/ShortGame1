#pragma once

#define CHUNK_SIZE 32
#define CHUNK_HEIGHT 128

namespace GEM
{
	/**
	So... A map. It's just a bunch of nodes spreaded infinitely around, but with a distinct height. But we not just showing this nodes. No. We build
	marchingCubes for them. The map, for now, is infinite. Lately, the will be a boundries or wrap, to glue the edges of a map.

	How do MarchingCubes build? That's an interesting thing. You take 8 closest nodes, forming cube and based on characteristics of thise nodes you
	create a cube, which helps you decide, which sides of a cube is actually intersected by a terrain. Then you creates a mesh, out of all this points you
	have found.

	But it's an expensive operation. So we don't whant to perform often. But when we actually should perform it? Whell, only the first time we a chunk of nodes
	and every time when this chunk is changed somehow. But once again we can be a little cheap. Changing of one node affecting only cubes, that contains this node
	And if this node doesn't change realy drasticly(don't change the state of a cube) then we can just tweak 6 edgepoints, that are connected to a node
	(3 edgepoints owned by this point, front edgepoint of a node behind, and top edgepoint of a node below, right edgepoint of a node to the left) and that's it!
	We don't even have to change Vertex/Index arrays!

	But what if change of state did occur? Whell, still only 8 cubes are affected, but now whe need to rebuild Index/Vertex lists. We can't just remove vertecies
	that missing edges were creating, becouse we reuse vertices over a mesh and they might be used by other cubes. But we can hold a list of active cubes, the ones
	that have nodes turned on and turned off. And then we can just reiterate on them!

	So the Nodes must have marks of update, ither they should or should not be updated. They also must have some mechanism to access the cubes that they occupy.
	
	The must be a vector of Changed Cubes. If any node of a cube has changed it's state, the cube must be put in that list;
	The same thing with nodes. But for any changes, not only if turn off/on occur;

	NORMALS!

	Cubes must also keep references to all edges, that are presist!

	When then update method get's called, firstly gubes get's processed. If cube lost it activenes(all nodes in a cube ither on or off) it must remove
	itself from active list. If change wasn't that big it must just recalculate it Characterisits.

	Then the Nodes. For every node that have been changed, all the edges around it must be changed.

	Then after all changed nodes are fixed, if Cubes where also changed, the Vertex/Index lists are also rebuilding. For every Cube we build triangles and 
	pass them through referenceList;

	But what if no cubes where Changed? Then whe must deduce all the Vertices that correspond to a Nodes that have been changed. To reset their position
	and recalculate their normals.

	That means, that the must be a list, that juxtaposes nodes to a Vertices. And it's a Reference list! yay! So we still have to traverse ReferenceList
	In any case. But it's not that big.

	So the idea is:

	You tell a mapSystem, where the player is.
	
	It decides, which chunks must be shown to a player.
	
	If there is a chunks, that must be 


	*/
}