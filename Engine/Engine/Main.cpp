#include "SystemClass.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pSCmdLine, int iCmdShow)
{
	SystemClass* system;
	bool result;
	
	system = new SystemClass();

	if(!system)
	{
		return 0;
	}

	result = system->Initialize();

	if(result)
	{
		system->Run();
	}

	system->Shutdown();

	delete system;
	system = 0;

	return 0;
}