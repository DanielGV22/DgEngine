#include "Precompiled.h"
#include "ModelManager.h"
#include "ModelIO.h"

using namespace DgEngine;
using namespace DgEngine::Graphics;

namespace
{
	std::unique_ptr<ModelManager> sModelManager;
}

void ModelManager::StaticInitialize(const std::filesystem::path& rootPath)
{
	ASSERT(sModelManager == nullptr, "ModelManager already initialized.");
	sModelManager = std::make_unique<ModelManager>();
	sModelManager->SetRootDirectory(rootPath);
}

void ModelManager::StaticTerminate()
{
	sModelManager.reset();
}

ModelManager* ModelManager::Get()
{
	ASSERT(sModelManager != nullptr, "ModelManager not initialized.");
		return sModelManager.get();
}

void ModelManager::SetRootDirectory(const std::filesystem::path& rootPath)
{
	mRootDirectory = rootPath;
}

ModelId ModelManager::GetModelId(const std::filesystem::path& filePath) const
{
	return std::filesystem::hash_value(mRootDirectory / filePath);
}

ModelId ModelManager::LoadModel(const std::filesystem::path& filePath)
{
	const ModelId modelId = GetModelId(filePath);
	auto [iter, success] = mInventory.insert({ modelId, nullptr });
	if (success)
	{
		std::filesystem::path fullPath = mRootDirectory / filePath;
		auto& modelPtr = iter->second;
		modelPtr = std::make_unique<Model>();
		ModelIO::LoadModel(fullPath, *modelPtr);
		ModelIO::LoadMaterial(fullPath, *modelPtr);
	}
	return modelId;
}

const Model* ModelManager::GetModel(ModelId id)
{
	auto model = mInventory.find(id);
	if (model != mInventory.end())
	{
		return model->second.get();
	}
	return nullptr;
}
