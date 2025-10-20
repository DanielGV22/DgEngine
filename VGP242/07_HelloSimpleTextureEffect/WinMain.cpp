#include"GameState.h"

using namespace DgEngine;

int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int )
{
	
	AppConfig config;
	config.appName = L"Hello Simple Texture Effect";


	App& myApp = MainApp();
	myApp.AddState<SolarSystemState>("GameState");
	myApp.Run(config);

	return 0;
}