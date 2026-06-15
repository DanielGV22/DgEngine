#pragma once

#include <DgEngine/Inc/TypeIds.h>

enum class CustomComponentId
{
	CustomDebugDraw = static_cast<int>(DgEngine::ComponentId::Count),
	Fish,
	Fishing
};

enum class CustomServiceId
{
	CustomDebugDrawDisplay = static_cast<int>(DgEngine::ServiceId::Count),
	Fish
};