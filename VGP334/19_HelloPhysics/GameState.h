#pragma once

#include"DgEngine/Inc/DgEngine.h"

class GameState : public DgEngine::AppState 
{
public:
	void Initialize() override;
	void Terminate() override;
	void Update(float deltaTime) override;
	void Render() override;
	void DebugUI() override;

private:
	

	void UpdateCamera(float deltaTime);


	DgEngine::Graphics::Camera mCamera;
	DgEngine::Graphics::DirectionalLight mDirectionalLight;

	DgEngine::Graphics::RenderObject mBallObject;
	DgEngine::Physics::CollisionShape mBallShape;
	DgEngine::Physics::RigidBody mBallRigidBody;

	DgEngine::Graphics::RenderObject mGroundObject;
	DgEngine::Physics::CollisionShape mGroundShape;
	DgEngine::Physics::RigidBody mGroundRigidBody;

	DgEngine::Graphics::StandardEffect mStandardEffect;

};
 


