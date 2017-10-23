#pragma once
namespace GEM
{
	/**!
	Base class for NodesToMCGenerators
	*/
	class NTMCG_Base
	{
	protected:
		//DimXZ - Dimensions X and Z of a NodeChunk. Just in case if they would change. DimY is the same, but for Y		
		int DimXZ, DimY;

		int m_chunkPosX, m_chunkPosZ;
	public:

		NTMCG_Base(int _dimXZ, int _dimY, int _chunkPosX, int _chunkPosZ) :
			DimXZ(_dimXZ),
			DimY(_dimY),
			m_chunkPosX(_chunkPosX),
			m_chunkPosZ(_chunkPosZ)
		{}

		/**!
		Creates MC anew. It clears all buffers and starts from the beginning.
		*/
		virtual void Generate() = 0;

		/**!
		Updates Index and Vertex to match current NodeMap.
		It assumes, that Generate were allready called at least once
		*/
		virtual void Update() = 0;

		/**!
		Mark node as changed, so that it's cubes can be updated in Update
		*/
		virtual void ChangeNode(int x, int y, int z) =0;



		struct MidPointBase
		{
			float x = 0, y = 0, z = 0;
			float nx = 0, ny = 1, nz = 0;

			//Texture cordinate are actually 3d. But with a help of a flavors ony 2 of cordinate will end in actual mesh
			float uvx = 0, uvy = 0, uvz = 0;
			//This also means that MCToMesh should be able to work differetiate flavors correctly, choosing right u, v pair for a given flavor.
			enum Flavor { FLAVOR_UPDOWN, FLAVOR_LEFTRIGHT, FLAVOR_FRONTBACK } flavor;
		};

		int const getChunkX();
		int const getChunkZ();

		virtual MidPointBase* getVertexVectorElement(int i) = 0;
		virtual const int getVertexVectorSize() = 0;

		virtual int getIndexVectorElement(int i) = 0;
		virtual const int getIndexVectorSize() = 0;

		virtual ~NTMCG_Base() {};
	};
}