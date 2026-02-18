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

	struct BoxData
	{
		DgEngine::Graphics::RenderObject box;
		DgEngine::Physics::CollisionShape shape;
		DgEngine::Physics::RigidBody rigidBody;
	};
	using Boxes = std::vector<BoxData>;
	Boxes mBoxes;

	// cloth info
	DgEngine::Graphics::RenderObject mCloth;
	DgEngine::Graphics::Mesh mClothMesh;
	DgEngine::Physics::SoftBody mClothSoftBody;

	DgEngine::Graphics::StandardEffect mStandardEffect;

};
 


