#include "Precompiled.h"
#include "PhysicsWorld.h"
#include "PhysicsObject.h"

#include <omp.h>
#include <chrono>

using namespace DgEngine;
using namespace DgEngine::Physics;

namespace
{
	std::unique_ptr<PhysicsWorld> sPhysicsWorld;
}

void PhysicsWorld::StaticInitialize(const Settings& settings)
{
	ASSERT(sPhysicsWorld == nullptr, "PhysicsWorld: Already initialized");
	sPhysicsWorld = std::make_unique<PhysicsWorld>();
	sPhysicsWorld->Initialize(settings);
}
void PhysicsWorld::StaticTerminate()
{
	if (sPhysicsWorld != nullptr)
	{
		sPhysicsWorld->Terminate();
		sPhysicsWorld.reset();
	}
}
PhysicsWorld* PhysicsWorld::Get()
{
	ASSERT(sPhysicsWorld != nullptr, "PhysicsWorld: must be initialized");
	return sPhysicsWorld.get();
}
PhysicsWorld::~PhysicsWorld()
{
	ASSERT(mDynamicsWorld == nullptr, "PhysicsWorld: must be terminated");
}
void PhysicsWorld::Initialize(const Settings& settings)
{
	mSettings = settings;
	mInterface = new btDbvtBroadphase();
	mSolver = new btSequentialImpulseConstraintSolver();

#ifdef USE_SOFT_BODY
	mCollisionConfiguration = new btSoftBodyRigidBodyCollisionConfiguration();
	mDispatcher = new btCollisionDispatcher(mCollisionConfiguration);
	mDynamicsWorld = new btSoftRigidDynamicsWorld(mDispatcher, mInterface, mSolver, mCollisionConfiguration);
#else
	mCollisionConfiguration = new btDefaultCollisionConfiguration();
	mDispatcher = new btCollisionDispatcher(mCollisionConfiguration);
	mDynamicsWorld = new btDiscreteDynamicsWorld(mDispatcher, mInterface, mSolver, mCollisionConfiguration);
#endif

	mDynamicsWorld->setGravity(TobtVector3(mSettings.gravity));
	mDynamicsWorld->setDebugDrawer(&mPhysicsDebugDraw);
}

void PhysicsWorld::Terminate()
{
	SafeDelete(mDynamicsWorld);
	SafeDelete(mDispatcher);
	SafeDelete(mCollisionConfiguration);
	SafeDelete(mSolver);
	SafeDelete(mInterface);
}
void PhysicsWorld::Update(float deltaTime)
{
	mDynamicsWorld->stepSimulation(deltaTime, mSettings.simulationSteps, mSettings.fixedTimeStep);

	const auto syncStart = std::chrono::high_resolution_clock::now();

	if (mUseParallelSync && mLastObjectCount > 64)
	{
		mParallelSyncTimeMs = mLastSyncTimeMs;

		if (mSerialSyncTimeMs > 0.0f && mParallelSyncTimeMs > 0.0f)
		{
			mSpeedup = mSerialSyncTimeMs / mParallelSyncTimeMs;
		}
	}
	else
	{
		mSerialSyncTimeMs = mLastSyncTimeMs;

		if (mSerialSyncTimeMs > 0.0f && mParallelSyncTimeMs > 0.0f)
		{
			mSpeedup = mSerialSyncTimeMs / mParallelSyncTimeMs;
		}
	}

	mLastObjectCount = static_cast<int>(mPhysicsObjects.size());

	if (mUseParallelSync && mLastObjectCount > 64)
	{
		mLastThreadCount = omp_get_max_threads();

		// VGP340 Final Project:
        // Parallelize the engine-side physics synchronization stage.
        // Bullet performs the simulation first, then OpenMP distributes
        // transform synchronization across registered physics objects.
#pragma omp parallel for
		for (int i = 0; i < mLastObjectCount; ++i)
		{
			mPhysicsObjects[i]->SyncWithGraphics();
		}
	}
	else
	{
		mLastThreadCount = 1;

		for (PhysicsObject* obj : mPhysicsObjects)
		{
			obj->SyncWithGraphics();
		}
	}

	const auto syncEnd = std::chrono::high_resolution_clock::now();
	mLastSyncTimeMs = std::chrono::duration<float, std::milli>(syncEnd - syncStart).count();
}
void PhysicsWorld::DebugUI()
{
	if (ImGui::CollapsingHeader("Physics", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::DragFloat("Gravity", &mSettings.gravity.x, 0.1f))
		{
			mDynamicsWorld->setGravity(TobtVector3(mSettings.gravity));
		}
		ImGui::Checkbox("DebugDraw", &mDebugDraw);
		if (mDebugDraw)
		{
			ImGui::Indent();

			int debugMode = mPhysicsDebugDraw.getDebugMode();
			bool drawWireFrame = (debugMode & btIDebugDraw::DBG_DrawWireframe);
			if (ImGui::Checkbox("DrawWireFrame", &drawWireFrame))
			{
				debugMode = (drawWireFrame) ? (debugMode | btIDebugDraw::DBG_DrawWireframe) : debugMode & ~btIDebugDraw::DBG_DrawWireframe;
			}
			bool drawAABB = (debugMode & btIDebugDraw::DBG_DrawAabb);
			if (ImGui::Checkbox("DrawAABB", &drawAABB))
			{
				debugMode = (drawAABB) ? debugMode | btIDebugDraw::DBG_DrawAabb : debugMode & ~btIDebugDraw::DBG_DrawAabb;
			}
			bool drawContactPoints = (debugMode & btIDebugDraw::DBG_DrawContactPoints);
			if (ImGui::Checkbox("DrawContactPoints", &drawContactPoints))
			{
				debugMode = (drawContactPoints) ? debugMode | btIDebugDraw::DBG_DrawContactPoints : debugMode & ~btIDebugDraw::DBG_DrawContactPoints;
			}
			mPhysicsDebugDraw.setDebugMode(debugMode);
			mDynamicsWorld->debugDrawWorld();

			ImGui::Unindent();
		}
		ImGui::Separator();
		ImGui::Text("VGP340 Parallel Physics Sync");
		ImGui::Checkbox("Use Parallel Sync", &mUseParallelSync);
		ImGui::Text("Physics Objects: %d", mLastObjectCount);
		ImGui::Text("Sync Time: %.4f ms", mLastSyncTimeMs);
		ImGui::Text("OpenMP Threads: %d", mLastThreadCount);

		if (mUseParallelSync)
		{
			ImGui::Text("Mode: Parallel OpenMP");
		}
		else
		{
			ImGui::Text("Mode: Serial");
		}
		ImGui::Text("Last Serial Sync: %.4f ms", mSerialSyncTimeMs);
		ImGui::Text("Last Parallel Sync: %.4f ms", mParallelSyncTimeMs);
		ImGui::Text("Estimated Speedup: %.2fx", mSpeedup);     
	}
}

void PhysicsWorld::UpdateSettings(const Settings& settings)
{
	mSettings = settings;
	SetGravity(settings.gravity);
}

void PhysicsWorld::SetGravity(const Math::Vector3& gravity)
{
	mSettings.gravity = gravity;
	mDynamicsWorld->setGravity(TobtVector3(gravity));
}
void PhysicsWorld::Register(PhysicsObject* physicsObject)
{
	auto iter = std::find(mPhysicsObjects.begin(), mPhysicsObjects.end(), physicsObject);
	//if iter is the end, it is NOT in the list, Register means we want to add it, so it is safe to add now
	if (iter == mPhysicsObjects.end())
	{
		mPhysicsObjects.push_back(physicsObject);
#ifdef USE_SOFT_BODY
		if (physicsObject->GetSoftBody() != nullptr)
		{
			mDynamicsWorld->addSoftBody(physicsObject->GetSoftBody());
		}
#endif
		if (physicsObject->GetRigidBody() != nullptr)
		{
			mDynamicsWorld->addRigidBody(physicsObject->GetRigidBody());
		}
	}
}

void PhysicsWorld::Unregister(PhysicsObject* physicsObject)
{
	auto iter = std::find(mPhysicsObjects.begin(), mPhysicsObjects.end(), physicsObject);
	//if iter is NOT the end, it IS in the list, unregister means we want to remove it, we found it so we can remove it now
	if (iter != mPhysicsObjects.end())
	{
#ifdef USE_SOFT_BODY
		if (physicsObject->GetSoftBody() != nullptr)
		{
			mDynamicsWorld->removeSoftBody(physicsObject->GetSoftBody());
		}
#endif
		if (physicsObject->GetRigidBody() != nullptr)
		{
			mDynamicsWorld->removeRigidBody(physicsObject->GetRigidBody());
		}
		mPhysicsObjects.erase(iter);
	}
}