#include"Precompiled.h"
#include"App.h"
#include"AppState.h"

using namespace DgEngine;
using namespace DgEngine::Core;
using namespace DgEngine::Graphics;
using namespace DgEngine::Input;

void App::Run(const AppConfig& config)
{
	LOG("App Started");

	Window myWindow;
	myWindow.Initialize(
		GetModuleHandle(nullptr),
		config.appName,
		config.winWidth,
	    config.winHeight
	);
	auto handle = myWindow.GetWindowHandle();
	GraphicsSystem::StaticInitialize(handle, false);
	InputSystem::StaticInitialize(handle);


	// last step before running
	ASSERT(mCurrentState != nullptr, "App: need an app state to run");
	mCurrentState->Initialize();

	// Process updates

	InputSystem* input= InputSystem::Get();
	mRunning = true;
	while (mRunning) 
	{
		myWindow.ProcessMessage();
		input->Update();

		if (!myWindow.IsActive() || input->IsKeyPressed(KeyCode::ESCAPE)) 
		{
			Quit();
			continue;
		}

		if(mNextState != nullptr)
		{
			mCurrentState->Terminate();
			mCurrentState = std::exchange(mNextState, nullptr);
			mCurrentState->Initialize();
		}

		float deltaTime = TimeUtil::GetDeltaTime();
 #if defined(_DEBUG)
		if (deltaTime < 0.5f)// primarily for handling breakpoints 
#endif
		{
			mCurrentState->Update(deltaTime);
		}

		GraphicsSystem* gs= GraphicsSystem::Get();
		gs->BeginRender();
		    mCurrentState->Render();
		gs->EndRender();
	}

	// Terminate everything
	LOG("App Quit");
	mCurrentState->Terminate();

	InputSystem::StaticTerminate();
	GraphicsSystem::StaticTerminate();
	myWindow.Terminate();
}

void App::Quit()
{
	mRunning = false;

}

void App::ChangeState(const std::string& stateName)
{
	auto iter = mAppStates.find(stateName);
	if(iter != mAppStates.end())
	{
		mNextState = iter->second.get();
	}
}
