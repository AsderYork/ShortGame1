#pragma once
#pragma once
#include <OGRE\OgreLogManager.h>
#include <OGRE\OgreLog.h>
namespace GEM
{
	/*!This class redirects OgreLog messages to LogForCpp*/
	class OgreLogToLogForCpp : public Ogre::LogListener
	{
	public:
		/*!Implementation of a virtual function from Ogre::LogListener. This function is used by Ogre to rederect messages. You can find more about it
		in Ogre API reference.*/
		void messageLogged(const Ogre::String& message, Ogre::LogMessageLevel lml, bool maskDebug, const Ogre::String& logName, bool& skipThisMessage);

		/*!This function, no matter what, creates Ogre::LogManager. Then it creates a log and subscribe
		itself as LogListener. It can be used only once. \return Returns true if succeed and false otherwise*/
		void connectToOgreLog();

		~OgreLogToLogForCpp();

	private:
		/*! A log, to witch this listener is subscribed*/
		std::unique_ptr<Ogre::Log> boundedLog;

		/*! Well, we created a logManager, and so we own it. This is representation of said fact*/
		std::unique_ptr<Ogre::LogManager> boundedLogManager;
	};
}