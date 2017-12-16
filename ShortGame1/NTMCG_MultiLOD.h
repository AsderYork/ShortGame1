#pragma once
#include "NodeChunk.h"
#include <memory>
#include <vector>
#include <list>
#include <OGRE\OgreVector3.h>

#define NTMCG_MAX_LOD_LVL 5

namespace GEM
{
	/**!
	Creates a MarchingCubes vertex vector out of 4 chunks. It's capable of aproximating chunk information to reduce maximum polygin count
	for it representation.
	*/
	class NTMCG_MultiLOD
	{
	public:

		struct Vertex {
			//Position and normal for a given vertex. In previous versions there also was a textcord, but it's actually just deduced from position
			Ogre::Vector3 pos, normal;
			//Flavor should be used in a pair with vertex to help deduce textcord
			enum class Flavor { TOPDOWN, BACKFRONT, LEFTRIGHT };
		};
	private:
		//Values from that paper about MarchingCubes
		static const int triTable[256][16];
		//Dimensions of a chunk.
		int DimXZ, DimY;
		//If nodes = 1 then evry node will be represented. The bigger is m_lod, the more aprroximate will be MarchingCubes
		int m_lod = 1;

		/*!An array of chunks. Center, front, right, frontright. It's possible to create a class enum for holding this values, but I don't realy
		think it is needed. Hope Im right.*/
		std::shared_ptr<GEM::NodeChunk> m_chunks[4];

		/**!
		MultiLOD can process single chunk with various levels of detail. m_cache holds results of processing
		for every level. It also holds bool value to show, if cachesd version is up to date.
		So as soon as any chenges is done and at least one new LOD for changed chunk is generated/updated, every LODs that wasn't updated
		must be marked as obsolete.
		*/
		std::vector<std::pair<std::vector<std::pair<Vertex*, Vertex::Flavor>>,bool>> m_cache;

	public:

		NTMCG_MultiLOD(std::shared_ptr<GEM::NodeChunk> Chunks[4], int _DimXZ, int _DimY) :
			DimXZ(_DimXZ),
			DimY(_DimY)

		{
			m_chunks[0] = Chunks[0];
			m_chunks[1] = Chunks[1];
			m_chunks[2] = Chunks[2];
			m_chunks[3] = Chunks[3];
		}




		

	};
}