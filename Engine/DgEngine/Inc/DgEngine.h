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

// services
#include "Service.h"
#include "CameraService.h"
#include "RenderService.h"

namespace DgEngine
{
	App& MainApp();

}