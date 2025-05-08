#include<DgEngine/Inc/DgEngine.h>

class MainState : public DgEngine::AppState
{
public:
	void Initialize()
	{
		LOG("MAIN STATE INITIALIZED");
		mLifeTime = 2.0f;
	}
	void Terminate()
	{
		LOG("MAIN STATE TERMINATED");
	}
	void Update(float deltaTime) override
	{
		mLifeTime -= std::max(deltaTime,0.01f);
		if(mLifeTime <= 0.0f)
		{
		   			DgEngine::MainApp().ChangeState("GameState");
		}
	}
private:
	float mLifeTime = 0.0f;
};

class GameState : public DgEngine::AppState
{
public:
	void Initialize()override
	{
		LOG("GAME STATE INITIALIZED");
		mLifeTime = 2.0f;

	}
	void Terminate() override
	{
		LOG("GAME STATE TERMINATED");
	}
	void Update(float deltaTime) override
	{
		mLifeTime -= std::max(deltaTime, 0.01f);
		if (mLifeTime <= 0.0f)
		{
			DgEngine::MainApp().ChangeState("MainState");
		}
	}
private:
	float mLifeTime = 0.0f;

};

int WINAPI WinMain(HINSTANCE instance, HINSTANCE , LPSTR , int )
{
	DgEngine::AppConfig config;
	config.appName = L"Hello Window";

	DgEngine::App& myApp= DgEngine::MainApp();
	myApp.AddState<MainState>("MainState");
	myApp.AddState<GameState>("GameState");
	myApp.Run(config);


	return 0;
}