#include "stdafx.h"
#include "Server.h"
#include <BufferStream.h>


int main(int argc, char *argv[])
{
	GEM::logHelper::setLog("./logs/log.txt");
	//GEM::GameSim::DoPhysics();

	std::stringstream stsrs;
	{
		std::size_t q = 128;
		std::size_t v = 128;
		cereal::BinaryOutputArchive ar(stsrs);
		ar(q,v);
	}
	std::string RealData = stsrs.str();

	{
		GEM::BufferIStream rtds(RealData);

		std::size_t q;
		std::size_t v;
		cereal::BinaryInputArchive ar(rtds);
		ar(q);
	}

	{
		GEM::BufferIStream rtds(RealData);

		std::size_t q;
		cereal::BinaryInputArchive ar(rtds);
		ar(q);
	}
	
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