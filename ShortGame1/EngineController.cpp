#include "stdafx.h"
#include "EngineController.h"

int GEM::EngineController::start()
{
	if (!initializeServices()){
		shutdownServices();
		LOGCATEGORY("EngineController/start").crit("Can't even initialize services. Terminating.");
		return 0;
	}

	//Main loop!
	double timeDelta = 1;
	bool TerminateUnexpected = false;//Set to true if termination was not becouse of terminate() call;
	while(!m_shouldTerminate)
	{
		if (!doPreFrame(timeDelta)) { TerminateUnexpected = true; break; }
		if (!doFrame(timeDelta)) { TerminateUnexpected = true; break; }
		if (!doPostFrame(timeDelta)) { TerminateUnexpected = true; break; }
	}


	shutdownServices();
	return 0;
}

void GEM::EngineController::shutdown()
{
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
