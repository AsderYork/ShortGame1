#include "Service.h"
#include "..\..\logHelper\logHelper.h"

namespace GEMClient
{
	GEM_Service::GEM_Service()
	{
	}

	GEM_Service::~GEM_Service()
	{

	}

	Service_Controller * GEM_Service::getController()
	{
		return controller;
	}

	Service_Controller::Service_Controller()
	{
		GEM::logHelper::setLog();
	}

	Service_Controller::~Service_Controller()
	{
		Input.Destroy();
		ServicesVector.clear();
		GEMCDS_Network_Client.Stop();
	}

	int Service_Controller::Start()
	{		
		int BreakeOnInit = -1;
		for (int i=0; i<ServicesVector.size(); i++)
		{

			if (ServicesVector[i]->Init() == false)
			{
				BreakeOnInit = i;
				LOGCATEGORY("ServiceController/start").error("One of the services failed initialization! Shuting down all initialized services and terminate");
				break;
			}
		}
		if (BreakeOnInit != -1)
		{
			for (int i = BreakeOnInit; i >= 0; i--)
			{
				ServicesVector[i]->Shutdown();
			}
			return TERMINATE_REASON::TR_INIT;
		}

		//A FRAME!!!
		//Initialization of tools
		if (!StartTools()) {
			LOGCATEGORY("ServiceController/start").error("Tools failed to initialize!");
			Shutdown();
		}

		Input.addKeyboardListener("GUI_Console",&Console);

		TERMINATE_REASON ErrorTriflag = TR_GOOD;

		double TimeDelta = 0.3;//Default time for the first frame
		auto PreFrameTime = std::chrono::high_resolution_clock::now();

		while ((!ShutdownFlag) && (ErrorTriflag == TR_GOOD))
		{
			PreFrameTime = std::chrono::high_resolution_clock::now();
			Input.capture();
			Console.DoConsole();
			for (auto& Services : ServicesVector)
			{
				if (!Services->PreFrame(TimeDelta))
				{ 
					ErrorTriflag = TR_PREFRAME;
					LOGCATEGORY("ServiceController/start").error("Error in PreFrame!");
					break;
				}
			}
			if (ErrorTriflag != 0) { break; }

			for (auto& Services : ServicesVector)
			{
				if (!Services->Frame(TimeDelta))
				{
					ErrorTriflag = TR_FRAME;
					LOGCATEGORY("ServiceController/start").error("Error in Frame!");
					break;
				}
			}
			if (ErrorTriflag != 0) { break; }

			for (auto& Services : ServicesVector)
			{
				if (!Services->PostFrame(TimeDelta))
				{
					ErrorTriflag = TR_POSTFRAME;
					LOGCATEGORY("ServiceController/start").error("Error in PostFrame!");
					break;
				}
			}
			if (ErrorTriflag != 0) { break; }

			//Even though it looks scary, we just takeing a difference between start of the frame and current time, then cast it to seconds, and the save it to double
			TimeDelta = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - PreFrameTime).count();

		}
		LOGCATEGORY("ServiceController/start").info("Shuting down peacefully");
		
		GEMCDS_Network_Client.Stop();
		for (auto Service = ServicesVector.rbegin(); Service != ServicesVector.rend(); Service++) { (*Service)->Shutdown(); }

		return ErrorTriflag;
	}

	void Service_Controller::Shutdown()
	{
		ShutdownFlag = true;
	}


	bool Service_Controller::StartTools()
	{
		return true;
	}

	
}
