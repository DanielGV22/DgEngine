#include "Precompiled.h"
#include "RenderObject.h"

using namespace DgEngine;
using namespace DgEngine::Graphics;

void RenderObject::Terminate()
{
	meshBuffer.Terminate();
	TextureManager* tm = TextureManager::Get();
    tm->ReleaseTexture(diffuseMapId);
	tm->ReleaseTexture(specMapId);
	tm->ReleaseTexture(normalMapId);
	tm->ReleaseTexture(bumpMapId);
	
	TextureManager* tm2 = TextureManager::Get();
    tm2->ReleaseTexture(diffuseMapId);
	tm2->ReleaseTexture(specMapId);
	tm2->ReleaseTexture(normalMapId);
	tm2->ReleaseTexture(bumpMapId);
}