#include "stdafx.h"
#include "EngineController.h"


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
	bool TerminateUnexpected = false;//Set to true if termination was not becouse of terminate() call;
	while(!m_shouldTerminate)
	{
		if (!doPreFrame(timeDelta)) { TerminateUnexpected = true; break; }
		if (!doFrame(timeDelta)) { TerminateUnexpected = true; break; }
		if (!doPostFrame(timeDelta)) { TerminateUnexpected = true; break; }
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
