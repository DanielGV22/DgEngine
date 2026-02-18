#include "Precompiled.h"
#include "SoftBody.h"
#include "PhysicsWorld.h"

using namespace DgEngine;
using namespace DgEngine::Physics;
using namespace DgEngine::Graphics;

SoftBody::~SoftBody()
{
	ASSERT(mSoftBody == nullptr, "SoftBody: terminate must be called!");
}

void SoftBody::Initialize(Graphics::Mesh& mesh, float mass, const std::vector<uint32_t>& fixedNodeIndices, bool linkEdges)
{
#ifdef USE_SOFT_BODY
	btSoftRigidDynamicsWorld* world = PhysicsWorld::Get()->GetSoftBodyWorld();
	ASSERT(world != nullptr, "SoftBody: soft body world does not exist");
	mSoftBody = new btSoftBody(&world->getWorldInfo(), mesh.vertices.size(), nullptr, nullptr);
#else
	ASSERT(false, "SoftBody: called when SoftBody is DISABLED");
	return;
#endif
	mMesh = &mesh;
	btSoftBody::Material* pm = mSoftBody->appendMaterial();
	pm->m_kAST = 0.8f;
	pm->m_kLST = 0.8f;
	pm->m_kVST = 0.8f;
	mSoftBody->m_cfg.kMT = 0.2f;
	mSoftBody->m_cfg.kCHR = 1.0f;
	mSoftBody->m_cfg.piterations = 10;
	mSoftBody->m_cfg.viterations = 10;
	mSoftBody->m_cfg.diterations = 10;
	mSoftBody->m_cfg.collisions |= btSoftBody::fCollision::SDF_RS;
	for (size_t i = 0; i < mesh.vertices.size(); ++i)
	{
		mSoftBody->m_nodes[i].m_x = TobtVector3(mesh.vertices[i].position);
	}
	for (size_t i = 0; i < mesh.indices.size(); i += 6)
	{
		int index0 = mesh.indices[i];
		int index1 = mesh.indices[i + 4];
		int index2 = mesh.indices[i + 5];
		int index3 = mesh.indices[i + 1];

		mSoftBody->appendLink(index0, index1);
		mSoftBody->appendLink(index1, index2);
		mSoftBody->appendLink(index2, index3);
		mSoftBody->appendLink(index3, index0);
		mSoftBody->appendLink(index1, index3);
		mSoftBody->appendLink(index0, index2);
	}
	for (const int& fixedNode : fixedNodeIndices)
	{
		mSoftBody->m_nodes[fixedNode].m_im = 0.0f;
	}
	PhysicsWorld::Get()->Register(this);

}

void SoftBody::Terminate()
{
	PhysicsWorld::Get()->Unregister(this);
	SafeDelete(mSoftBody);
}

void SoftBody::SyncWithGraphics()
{
	for (size_t i = 0; i < mMesh->vertices.size(); ++i)
	{
		mMesh->vertices[i].position = ToVector3(mSoftBody->m_nodes[i].m_x);
	}
}

btSoftBody* SoftBody::GetSoftBody()
{
	return mSoftBody;
}