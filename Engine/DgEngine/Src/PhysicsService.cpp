#include "Precompiled.h"
#include "PhysicsService.h"
#include "RigidBodyComponent.h"
#include "SaveUtil.h"

using namespace DgEngine;

void PhysicsService::Update(float deltaTime)
{
	if (mEnabled)
	{
		Physics::PhysicsWorld::Get()->Update(deltaTime);
	}
}

void PhysicsService::DebugUI()
{
	if (mEnabled)
	{
		Physics::PhysicsWorld::Get()->DebugUI();
	}
}

void PhysicsService::Deserialize(const rapidjson::Value& value)
{
	Physics::PhysicsWorld::Settings settings;
	int simSteps = settings.simulationSteps;
	SaveUtil::ReadVector3("Gravity", settings.gravity, value);
	SaveUtil::ReadInt("SimSteps", simSteps, value);
	SaveUtil::ReadFloat("FixedTimeStep", settings.fixedTimeStep, value);
	settings.simulationSteps = simSteps;
	Physics::PhysicsWorld::Get()->UpdateSettings(settings);
}

void PhysicsService::Serialize(rapidjson::Document& doc, rapidjson::Value& value)
{
	rapidjson::Value serviceValue(rapidjson::kObjectType);

	const Physics::PhysicsWorld::Settings& settings =
		Physics::PhysicsWorld::Get()->GetSettings();

	int simSteps = settings.simulationSteps;

	SaveUtil::WriteVector3("Gravity", settings.gravity, doc, serviceValue);
	SaveUtil::WriteInt("SimSteps", simSteps, doc, serviceValue);
	SaveUtil::WriteFloat("FixedTimeStep", settings.fixedTimeStep, doc, serviceValue);

	value.AddMember("PhysicsService", serviceValue, doc.GetAllocator());
}


void PhysicsService::Register(RigidBodyComponent* rigidBodyComponent)
{
	Physics::PhysicsWorld::Get()->Register(&rigidBodyComponent->mRigidBody);
}

void PhysicsService::Unregister(RigidBodyComponent* rigidBodyComponent)
{
	Physics::PhysicsWorld::Get()->Unregister(&rigidBodyComponent->mRigidBody);
}

void PhysicsService::SetEnabled(bool enabled)
{
	mEnabled = enabled;
}