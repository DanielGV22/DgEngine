#include "Precompiled.h"
#include "PhysicsDebugDraw.h"

using namespace DgEngine;
using namespace DgEngine::Physics;
using namespace DgEngine::Graphics;

void PhysicsDebugDraw::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
	SimpleDraw::AddLine(ToVector3(from), ToVector3(to), ToColor(color));
}

void PhysicsDebugDraw::drawContactPoint(const btVector3& pointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
{
	SimpleDraw::AddLine(ToVector3(pointOnB), ToVector3(pointOnB + normalOnB), ToColor(color));
}

void PhysicsDebugDraw::reportErrorWarning(const char* warningString)
{
	LOG("[PhysicsErrorWarning] %s", warningString);
}

void PhysicsDebugDraw::draw3dText(const btVector3& location, const char* textString)
{
	// NOT IMPLEMENTED
}

void PhysicsDebugDraw::setDebugMode(int debugMode)
{
	mDebugMode = debugMode;
}

int PhysicsDebugDraw::getDebugMode() const
{
	return mDebugMode;
}