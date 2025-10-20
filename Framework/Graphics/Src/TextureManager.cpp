#include"Precompiled.h"
#include"TextureManager.h"

using namespace DgEngine;
using namespace DgEngine::Graphics;

namespace
{
	std::unique_ptr<TextureManager> sInstance;
}

void TextureManager::StaticInitialize(const std::filesystem::path& root)
{
	ASSERT(sInstance == nullptr, "TextureManager: is already initialized");
	sInstance = std::make_unique<TextureManager>();
	sInstance->SetRootDirectory(root);
}

void TextureManager::StaticTerminate()
{
	if (sInstance != nullptr)
	{
		sInstance.reset();
	}
}

TextureManager* TextureManager::Get()
{
	ASSERT(sInstance != nullptr, "TextureManager: is not initialized");
	return sInstance.get();
}

TextureManager::~TextureManager()
{
	ASSERT(mInventory.empty(), "TextureManager: not all textures are cleared");
}

void TextureManager::SetRootDirectory(const std::filesystem::path& root)
{
	mRootDirectory = root;
}

TextureId TextureManager::LoadTexture(const std::filesystem::path& fileName, bool useRootDir)
{
	const size_t textureId = std::filesystem::hash_value(fileName);
	auto [iter, success] = mInventory.insert({ textureId, Entry() });
	if (success)
	{
		iter->second.texture = std::make_unique<Texture>();
		iter->second.texture->Initialize((useRootDir) ? mRootDirectory / fileName : fileName);
		iter->second.refCount = 1;
	}
	else
	{
		++iter->second.refCount;
	}
	return textureId;
}

const Texture* TextureManager::GetTexture(TextureId id)
{
	auto iter = mInventory.find(id);
	if (iter != mInventory.end())
	{
		return iter->second.texture.get();
	}
	return nullptr;
}

void TextureManager::ReleaseTexture(TextureId id)
{
	auto iter = mInventory.find(id);
	if (iter != mInventory.end())
	{
		--iter->second.refCount;
		if (iter->second.refCount == 0)
		{
			iter->second.texture->Terminate();
			iter->second.texture.release();
			mInventory.erase(iter);
		}
	}
}

void TextureManager::BindVS(TextureId id, uint32_t slot) const
{
	auto iter = mInventory.find(id);
	if (iter != mInventory.end())
	{
		iter->second.texture->BindVS(slot);
	}
}

void TextureManager::BindPS(TextureId id, uint32_t slot) const
{
	auto iter = mInventory.find(id);
	if (iter != mInventory.end())
	{
		iter->second.texture->BindPS(slot);
	}
}

