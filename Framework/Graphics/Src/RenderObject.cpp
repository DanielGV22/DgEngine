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

void RenderGroup::Initialize(const std::filesystem::path& modelFilePath)
{
	modelId = ModelManager::Get()->LoadModel(modelFilePath);
	const Model* model = ModelManager::Get()->GetModel(modelId);
	ASSERT(model != nullptr, "RenderGroup: Model %s could not be loaded.", modelFilePath.u8string().c_str());

	for (const Model::MeshData& meshData : model->meshData)
	{
		RenderObject& RenderObject = renderObjects.emplace_back();
		RenderObject.meshBuffer.Initialize(meshData.mesh);
		if (meshData.materialIndex < model->materialData.size())
		{
			
		}
	}
}

void RenderGroup::Terminate()
{
	for (RenderObject& renderObject : renderObjects)
	{
		renderObject.Terminate();
	}
	renderObjects.clear();
}