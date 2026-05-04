#pragma once

#include"Common.h"

#include "App.h"
#include "AppState.h"
#include "SaveUtil.h"

// game object info
#include "GameObject.h"
#include "GameWorld.h"
#include "GameObjectHandle.h"
#include "GameObjectFactory.h"

// components
#include "TypeIds.h"
#include "Component.h"
#include "TransformComponent.h"
#include "CameraComponent.h"
#include "FPSCameraComponent.h"
#include "RenderObjectComponent.h"
#include "MeshComponent.h"
#include "ModelComponent.h"
#include "AnimatorComponent.h"
#include "RigidBodyComponent.h"

// services
#include "Service.h"
#include "CameraService.h"
#include "RenderService.h"
#include "PhysicsService.h"

#define USE_PHYSICS_SERVICE

namespace DgEngine
{
	App& MainApp();

}