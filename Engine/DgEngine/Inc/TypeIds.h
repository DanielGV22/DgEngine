#pragma once

namespace DgEngine
{
	enum class ComponentId
	{
		Invalid,            // default value, not valid Id
		Transform,			// transform component for location data
		Camera,				// contains the camera for viewing
		FPSCamera,			// move the Camera Component with FPS controls
		Mesh,				// add a mesh shape on the game object to render
		Model,				// add a model to the game object to render
		Animator,			// adds an animation controller to the game object
		RigidBody,			// adds a physics object to the game object
		Count               // last value, can be used to chain custom components
	};

	enum class ServiceId
	{
		Invalid,            // default value, not valid Id
		Camera,				// manages the cameras in the world
		Render,				// renders renderobjects in the world
		Physics, 		    // registers and mnitors physics objects in the world
		Count               // last value, can be used to chain custom services
	};
}

#define SET_TYPE_ID(id)\
	static uint32_t StaticGetTypeId() { return static_cast<uint32_t>(id); }\
	uint32_t GetTypeId() const override { return StaticGetTypeId(); }