#pragma once

#include <log4cpp/Category.hh>


namespace Helper
{

#define LOGROOT() log4cpp::Category::getRoot()

/**
\def LOGCATEGORY(CATEGORY_NAME)
\brief Provide access to a given log category
It is recomended to use it like \code{.cpp} LOGCATEGORY("CLASS/METHOD").warn("printf-like formatted message", params)\endcode.
There is more then warn/debug/error, and the way log messages are shown is controlled by \c setLog method.
\note You must call \c setLog method before first usage of this macro!
*/
#define LOGCATEGORY(CATEGORY_NAME) log4cpp::Category::getInstance(CATEGORY_NAME)


	/**
	\brief A method-boundle to set and controll log-file and console log output
	*/
	struct logHelper
	{
		/**
		\brief Sets file and conslole log appenders.
		param[in] path A path of log file. It might include only name of the file, then it'll be created near .exe. Or it can use absolue or reletive paths.
		It can be called multiple times, but every time it's starts writing log in a file, file get's cleaned.
		*/
		static void setLog(std::string path);
	};
}