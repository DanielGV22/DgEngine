#include<DgEngine/Inc/DgEngine.h>
#include"ShapeState.h"



int WINAPI WinMain(HINSTANCE instance, HINSTANCE , LPSTR , int )
{
	DgEngine::AppConfig config;
	config.appName = L"Hello Shapes";

	DgEngine::App& myApp= DgEngine::MainApp();
	myApp.AddState<ShapeState>("ShapeState");
	myApp.AddState<TriangleShapeState>("TriangleShapeState");
	myApp.AddState<SquareShapeState>("SquareShapeState");
	myApp.AddState<HouseShapeState>("HouseShapeState");
	myApp.AddState<RombusShapeState>("RombusShapeState");
	myApp.Run(config);


	return 0;
}