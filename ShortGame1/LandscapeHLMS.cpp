#include "stdafx.h"
#include "LandscapeHLMS.h"

namespace GEM
{

	LadnscapeHLMS::LadnscapeHLMS(Ogre::Archive* dataFolder, Ogre::ArchiveVec* libraryFolders) : HlmsPbs(dataFolder, libraryFolders)
	{
		mType = Ogre::HlmsTypes::HLMS_USER0;
		HlmsPbs::HlmsBufferManager::Hlms::mTypeName = "landscape";
		HlmsPbs::HlmsBufferManager::Hlms::mTypeNameStr = "landscape";


	}
}
