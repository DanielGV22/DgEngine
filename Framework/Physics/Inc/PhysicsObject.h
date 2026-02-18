#pragma once

//an objects that connects to the physics world
// must be inherited to register with the world

namespace DgEngine::Physics
{
	class PhysicsObject
	{
	public:
		PhysicsObject() = default;
		virtual ~PhysicsObject() = default;

	protected:
		friend class PhysicsWorld;

		// abstract function
		virtual void SyncWithGraphics() = 0;
		virtual btRigidBody* GetRigidBody() { return nullptr; }
		virtual btSoftBody* GetSoftBody() { return nullptr; }
	};
}