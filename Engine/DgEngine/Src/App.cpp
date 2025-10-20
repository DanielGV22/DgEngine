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

    // Initialize everything
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
    DebugUI::StaticInitialize(handle, false, true);
    SimpleDraw::StaticInitialize(config.maxVertexCount);
    TextureManager::StaticInitialize(L"../../Assets/Textures");

    // last step before running
    ASSERT(mCurrentState != nullptr, "App: need an app state to run.");
    mCurrentState->Initialize();

    // Process updates

    InputSystem* input = InputSystem::Get();
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

        if (mNextState != nullptr)
        {
            mCurrentState->Terminate();
            mCurrentState = std::exchange(mNextState, nullptr);
            mCurrentState->Initialize();
            mNextState = nullptr;
        }

        float deltaTime = TimeUtil::GetDeltaTime();
#if defined(_DEBUG)
        if (deltaTime < 0.5f) // primarily for handling breakpoints
#endif
        {
            mCurrentState->Update(deltaTime);
        }

        GraphicsSystem* gs = GraphicsSystem::Get();
        gs->BeginRender();
        mCurrentState->Render();
        DebugUI::BeginRender();
        mCurrentState->DebugUI();
        DebugUI::EndRender();
        gs->EndRender();
    }

    // Terminate everything
    LOG("App Quit");
    mCurrentState->Terminate();

    TextureManager::StaticTerminate();
    SimpleDraw::StaticTerminate();
    DebugUI::StaticTerminate();
    InputSystem::StaticTerminate();
    GraphicsSystem::StaticTerminate();
    myWindow.Terminate();
}

void DgEngine::App::Quit()
{
    mRunning = false;
}

void DgEngine::App::ChangeState(const std::string& stateName)
{
    auto iter = mAppStates.find(stateName);
    if (iter != mAppStates.end())
    {
        mNextState = iter->second.get();
    }
    else
    {
        LOG("App: State %s not found", stateName.c_str());
    }
}