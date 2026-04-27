#pragma once

#include"Common.h"

#include"App.h"
#include"AppState.h"

// game object info
#include "GameObject.h"
#include "GameWorld.h"
#include "GameObjectHandle.h"

// components
#include "TypeIds.h"
#include "Component.h"
#include "TransformComponent.h"
#include "CameraComponent.h"
#include "FPSCameraComponent.h"

// services
#include "Service.h"

namespace DgEngine
{
	App& MainApp();

}