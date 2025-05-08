#include<DgEngine/Inc/DgEngine.h>
#include"ShapeState.h"



int WINAPI WinMain(HINSTANCE instance, HINSTANCE , LPSTR , int )
{
	DgEngine::AppConfig config;
	config.appName = L"Hello Shapes";

	DgEngine::App& myApp= DgEngine::MainApp();
	myApp.AddState<ShapeState>("ShapeState");
	myApp.Run(config);


	return 0;
}