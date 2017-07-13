#include "logHelper.h"


#include <log4cpp/Appender.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/Layout.hh>
#include <log4cpp/BasicLayout.hh>
#include <log4cpp/PatternLayout.hh>
#include <log4cpp/Priority.hh>

namespace GEM
{
	void logHelper::setLog()
	{
		std::string LayoutString = "[%d{%d/%m/%Y %H:%M:%S;%l}: %p]%c:%m%n";
		auto logLayoutForConsole = new log4cpp::PatternLayout();
		logLayoutForConsole->setConversionPattern(LayoutString.c_str());

		auto l_consoleAppender = new log4cpp::OstreamAppender("console", &std::cout);
		l_consoleAppender->setLayout(logLayoutForConsole);

		auto logLayoutForFile = new log4cpp::PatternLayout();
		logLayoutForFile->setConversionPattern(LayoutString.c_str());

		auto l_fileAppender = new log4cpp::FileAppender("file", "lastStart.log", false);
		l_fileAppender->setLayout(logLayoutForFile);


		log4cpp::Category& root = log4cpp::Category::getRoot();
		root.setPriority(log4cpp::Priority::DEBUG);

		//This methods takes ownership;
		root.addAppender(l_consoleAppender);
		root.addAppender(l_fileAppender);
	}
	void logHelper::ReorderLogPath(std::string path)
	{
		log4cpp::Category::getRoot().removeAllAppenders();

		std::string LayoutString = "[%d{%d/%m/%Y %H:%M:%S;%l}: %p]%c:%m%n";
		auto logLayoutForConsole = new log4cpp::PatternLayout();
		logLayoutForConsole->setConversionPattern(LayoutString.c_str());

		auto logLayoutForFile = new log4cpp::PatternLayout();
		logLayoutForFile->setConversionPattern(LayoutString.c_str());

		auto l_consoleAppender = new log4cpp::OstreamAppender("console", &std::cout);
		l_consoleAppender->setLayout(logLayoutForConsole);

		auto l_fileAppender = new log4cpp::FileAppender("file", path, false);
		l_fileAppender->setLayout(logLayoutForFile);


		log4cpp::Category& root = log4cpp::Category::getRoot();

		//This methods takes ownership;
		root.addAppender(l_consoleAppender);
		root.addAppender(l_fileAppender);
	}

	void logHelper::setLog(std::string path)
	{
		std::string LayoutString = "[%d{%d/%m/%Y %H:%M:%S;%l}: %p]%c:%m%n";
		auto logLayoutForConsole = new log4cpp::PatternLayout();
		logLayoutForConsole->setConversionPattern(LayoutString.c_str());

		auto l_consoleAppender = new log4cpp::OstreamAppender("console", &std::cout);
		l_consoleAppender->setLayout(logLayoutForConsole);

		auto logLayoutForFile = new log4cpp::PatternLayout();
		logLayoutForFile->setConversionPattern(LayoutString.c_str());

		auto l_fileAppender = new log4cpp::FileAppender("file", path, false);
		l_fileAppender->setLayout(logLayoutForFile);


		log4cpp::Category& root = log4cpp::Category::getRoot();
		root.setPriority(log4cpp::Priority::DEBUG);

		//This methods takes ownership;
		root.addAppender(l_consoleAppender);
		root.addAppender(l_fileAppender);
	}

}