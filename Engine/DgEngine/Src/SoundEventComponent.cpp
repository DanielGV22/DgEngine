#include "Precompiled.h"
#include "SoundEventComponent.h"
#include "SaveUtil.h"

using namespace DgEngine;
using namespace DgEngine::Audio;

void SoundEventComponent::Initialize()
{
	ASSERT(!mFileName.empty(), "SoundEventComponent: no sound file loaded");
	mSoundId = SoundEffectManager::Get()->Load(mFileName);
}
void SoundEventComponent::Terminate()
{
	Stop();
}
void SoundEventComponent::DebugUI()
{
	ImGui::PushID(mFileName.c_str());
	ImGui::Text(mFileName.c_str());
	if (ImGui::Button("Play"))
	{
		Play();
	}

	ImGui::SameLine();
	if (ImGui::Button("Stop"))
	{
		Stop();
	}
	ImGui::PopID();
}
void SoundEventComponent::Deserialize(const rapidjson::Value& value)
{
	SaveUtil::ReadString("FileName", mFileName, value);
	SaveUtil::ReadBool("Looping", mLooping, value);
}

void SoundEventComponent::Serialize(rapidjson::Document& doc, rapidjson::Value& value, const rapidjson::Value& originalValue)
{
	rapidjson::Value componentValue(rapidjson::kObjectType);
	// compare with original, if different, save current value
	value.AddMember("SoundEventComponent", componentValue, doc.GetAllocator());
}

void SoundEventComponent::Play()
{
	SoundEffectManager::Get()->Play(mSoundId, mLooping);
}
void SoundEventComponent::Stop()
{
	SoundEffectManager::Get()->Stop(mSoundId);
}