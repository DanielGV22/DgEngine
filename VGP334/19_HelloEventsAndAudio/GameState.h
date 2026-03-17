#pragma once

#include"DgEngine/Inc/DgEngine.h"

class GameState : public DgEngine::AppState 
{
public:
	void Initialize() override;
	void Terminate() override;
	void Update(float deltaTime) override;
	void Render() override;
	void DebugUI() override;

private:
	

	void UpdateCamera(float deltaTime);

	void OnSpacePressedEvent(const DgEngine::Core::Event& e);

	DgEngine::Graphics::Camera mCamera;
	DgEngine::Graphics::ParticleSystemEffect mParticleSystemEffect;
	DgEngine::Physics::ParticleSystem mParticleSystem;
	DgEngine::Physics::ParticleSystem mFireworkParticles;

	DgEngine::Core::EventListenerId mSpacePressedListenerId = 0;
	DgEngine::Core::EventListenerId mEnterPressedListenerId = 0;

	DgEngine::Graphics::Animation mFireworkAnimation;
	float mFireworkAnimationTime = 0.0f;

	DgEngine::Audio::SoundId mLaunchSoundId = 0;
	DgEngine::Audio::SoundId mExplosionSoundId = 0;
};
 


