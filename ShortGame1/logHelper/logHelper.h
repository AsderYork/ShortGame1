#pragma once
/*!
A set of things, to make use of log4cpp easier
*/
#include <log4cpp/Category.hh>
#define LOGROOT() log4cpp::Category::getRoot()
#define LOGCATEGORY(CATEGORY_NAME) log4cpp::Category::getInstance(CATEGORY_NAME)

namespace GEM
{
	struct logHelper
	{
		static void setLog();
		static void ReorderLogPath(std::string path);
		static void setLog(std::string path);
	};
}