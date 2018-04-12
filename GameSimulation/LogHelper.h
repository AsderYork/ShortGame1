#pragma once

#include <log4cpp/Category.hh>

#define LOGROOT() log4cpp::Category::getRoot()
#define LOGCATEGORY(CATEGORY_NAME) log4cpp::Category::getInstance(CATEGORY_NAME)

namespace GEM
{
	/**!
	\brief A method-boundle to set and controll log-file and console log output
	*/
	struct logHelper
	{
		/**!
		\brief Sets file and conslole log appenders.
		param[in] path A path of log file. It might include only name of the file, then it'll be created near .exe. Or it can use absolue or reletive paths.
		It can be called multiple times, but every time it's starts writing log in a file, file get's cleaned.
		*/
		static void setLog(std::string path);
	};
}