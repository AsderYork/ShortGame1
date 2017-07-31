#include "stdafx.h"
#include "EngineController.h"
#include <chrono>

#define DEBUG_FRAME_AMOUNT 5000 //Amount of frames, that is counts


int GEM::EngineController::start()
{
	GEM::logHelper::setLog("laststart.log");
	LOGCATEGORY("EngineController/start").info("Log is set and start have begun!");

	if (!initializeServices()){
		shutdownServices();
		LOGCATEGORY("EngineController/start").crit("Can't even initialize services. Terminating.");
		return 0;
	}

	


	LOGCATEGORY("EngineController/start").info("Everything is ready for a main loop!");
	//Main loop!
	double timeDelta = 1;

	double DebugTime = 0;//Amount of time it takes to do 10000 frames
	int DebugFrames = 0;//Counter of frames;

	std::chrono::time_point<std::chrono::system_clock> start, end;

	bool TerminateUnexpected = false;//Set to true if termination was not becouse of terminate() call;
	while(!m_shouldTerminate)
	{
		start = std::chrono::system_clock::now();

		if (!doPreFrame(timeDelta)) { TerminateUnexpected = true; break; }
		if (!doFrame(timeDelta)) { TerminateUnexpected = true; break; }
		if (!doPostFrame(timeDelta)) { TerminateUnexpected = true; break; }

		end = std::chrono::system_clock::now();

		timeDelta = std::chrono::duration_cast<std::chrono::duration<double>>(end - start).count();

		DebugTime += timeDelta;
		DebugFrames++;

		if (DebugFrames >= DEBUG_FRAME_AMOUNT)
		{
			LOGCATEGORY("EngineController/start").info("%i frames were done in %f avg.", DEBUG_FRAME_AMOUNT, DebugTime/ DEBUG_FRAME_AMOUNT);
			DebugTime = 0;
			DebugFrames = 0;
		}

		if (timeDelta > 1) { timeDelta = 1; }//So that nothing would brake on debug stoppages
	}

	if(TerminateUnexpected)
	{//If reaction to normal and abnormal termination is practicly simmilar, then what's the point of differentiation?
		LOGCATEGORY("EngineController/start").error("Termination was unexpected. Probably something bad happened!");
	}


	LOGCATEGORY("EngineController/start").info("All work is done. Terminating!");
	shutdownServices();
	return 0;
}

void GEM::EngineController::shutdown()
{
	LOGCATEGORY("EngineController/shutdown").info("Shutdown gets called");
	m_shouldTerminate = true;
}

bool GEM::EngineController::initializeServices()
{
	for (auto& service : m_servicesVector)
	{
		if (service->initialize() != Service::AR_OK)
		{//Service fails to initialize!
			return false;
		}
		//If service succeed at initialization, then continue to the next one
	}//Untill they all get's initialized
	return true;
}

void GEM::EngineController::shutdownServices()
{
	//Going in reverse. Using reverse iterators
	for (auto service = m_servicesVector.rbegin(); service != m_servicesVector.rend(); service++)
	{
		(*service)->shutdown();
	}
}

bool GEM::EngineController::doPreFrame(double TimeDelta)
{
	for (auto& service : m_servicesVector)
	{
		if (service->preFrame(TimeDelta) != Service::AR_OK)
		{
			return false;
		}		
	}
	return true;
}

bool GEM::EngineController::doFrame(double TimeDelta)
{
	for (auto& service : m_servicesVector)
	{
		if (service->frame(TimeDelta) != Service::AR_OK)
		{
			return false;
		}
	}
	return true;
}

bool GEM::EngineController::doPostFrame(double TimeDelta)
{
	for (auto& service : m_servicesVector)
	{
		if (service->postFrame(TimeDelta) != Service::AR_OK)
		{
			return false;
		}
	}
	return true;
}
