#pragma once

namespace DgEngine
{
	enum class ComponentId
	{
		Invalid,            // default value, not valid Id
		Transform,			// transform component for location data
		Camera,				// contains the camera for viewing
		FPSCamera,			// move the Camera Component with FPS controls
		Count               // last value, can be used to chain custom components
	};

	enum class ServiceId
	{
		Invalid,            // default value, not valid Id
		Camera,				// manages the cameras in the world
		Count               // last value, can be used to chain custom services
	};
}

#define SET_TYPE_ID(id)\
	static uint32_t StaticGetTypeId() { return static_cast<uint32_t>(id); }\
	uint32_t GetTypeId() const override { return StaticGetTypeId(); }