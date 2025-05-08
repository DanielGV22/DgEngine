#include"Precompiled.h"
#include"DgEngine.h"

DgEngine::App& DgEngine::MainApp()
{
	static App sApp;
	return sApp;
}