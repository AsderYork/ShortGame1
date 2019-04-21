#pragma once
#include <OGRE\Hlms\Pbs\OgreHlmsPbs.h>
#include <OGRE\OgreHlmsListener.h>

namespace GEM
{
	/**Just inherits PBS implementation, all the work of shadering landscape is done in shaders */


	class LadnscapeHLMS : public Ogre::HlmsPbs
	{
	public:
		LadnscapeHLMS(Ogre::Archive* dataFolder, Ogre::ArchiveVec* libraryFolders);
	};
}
