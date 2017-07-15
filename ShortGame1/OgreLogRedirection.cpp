#include "stdafx.h"
#include "OgreLogRedirection.h"


namespace GEM
{

	void OgreLogToLogForCpp::messageLogged(const Ogre::String & message, Ogre::LogMessageLevel lml, bool maskDebug, const Ogre::String & logName, bool & skipThisMessage)
	{
		switch (lml)
		{
		case  Ogre::LogMessageLevel::LML_TRIVIAL: {LOGCATEGORY("OGRE").debug(message); break;}
		case  Ogre::LogMessageLevel::LML_NORMAL: {LOGCATEGORY("OGRE").info(message); break;}
		case  Ogre::LogMessageLevel::LML_CRITICAL: {LOGCATEGORY("OGRE").crit(message); break;}

		}
	}

	void OgreLogToLogForCpp::connectToOgreLog()
	{
		//If it's not true, then this function were called second time. This is not allowed
		assert(boundedLogManager.get() == nullptr);
		assert(boundedLog.get() == nullptr);

		boundedLogManager = std::make_unique<Ogre::LogManager>();

		boundedLog = std::unique_ptr<Ogre::Log>(boundedLogManager->createLog("OgreLogfile.log", false, false, true));
		boundedLog->setTimeStampEnabled(false);
		boundedLog->addListener(this);

	}

	OgreLogToLogForCpp::~OgreLogToLogForCpp()
	{
		boundedLog.release();
		boundedLogManager.release();
	}

}