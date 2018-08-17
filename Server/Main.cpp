#include "stdafx.h"
#include "Server.h"


int main(int argc, char *argv[])
{
	GEM::logHelper::setLog("./logs/log.txt");

	try
	{
		std::string Servername = "First one!";
		GEM::Server server(4483, GEM::ServerData(Servername));

		LOGCATEGORY("Main").info("Server' %s' is going to start right now!\n", Servername.c_str());
		server.Run();
	}
	catch (std::exception &e)
	{
		LOGCATEGORY("Main").crit("Exception reached main! Exception:%s", e.what());
	}
	catch(...)
	{
		LOGCATEGORY("Main").crit("Exception reached main! And it's not std::excpetion!");
	}

	return 0;
}