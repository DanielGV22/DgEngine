#include"GameState.h"

using namespace DgEngine;

int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int )
{
	
	AppConfig config;
	config.appName = L"Hello Post Processing";


	App& myApp = MainApp();
	myApp.AddState<GameState>("GameState");
	myApp.Run(config);

	return 0;
}