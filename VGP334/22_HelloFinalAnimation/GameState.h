#pragma once
#include "GameEvents.h"

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

    void RegisterEvents();
    void UnregisterEvents();
    void OnCycleAnimationEvent(const DgEngine::Core::Event& e);

    void DrawDiscoFloor();
    void DrawDiscoBall();
    void DrawClubLights();

    void ApplyCurrentAnimationSet();
    void PlayCurrentMusic();
    void TriggerCycleFX();
    void ResetScene();

private:
    // Camera / lighting
    DgEngine::Graphics::Camera mCamera;
    DgEngine::Graphics::DirectionalLight mDirectionalLight;
    DgEngine::Graphics::StandardEffect mStandardEffect;

    // Characters
    DgEngine::Graphics::RenderGroup mCharacterA;
    DgEngine::Graphics::Animator mAnimatorA;

    DgEngine::Graphics::RenderGroup mCharacterB;
    DgEngine::Graphics::Animator mAnimatorB;

    // Particles
    DgEngine::Graphics::ParticleSystemEffect mParticleSystemEffect;
    DgEngine::Physics::ParticleSystem mCenterParticles;
    DgEngine::Physics::ParticleSystem mLeftParticles;
    DgEngine::Physics::ParticleSystem mRightParticles;

    // Animation file lists
    std::vector<std::wstring> mCharacterAAnimSets;
    std::vector<std::wstring> mCharacterBAnimSets;

    // Music tracks
    std::vector<DgEngine::Audio::SoundId> mMusicTracks;

    DgEngine::Audio::SoundId mCurrentMusic = 0;
    DgEngine::Audio::SoundId mBurstSoundId = 0;

    float mDiscoBallRotation = 0.0f;

    float mLightPulse = 0.0f;
    DgEngine::Graphics::Color mClubColorA = DgEngine::Graphics::Colors::Magenta;
    DgEngine::Graphics::Color mClubColorB = DgEngine::Graphics::Colors::Cyan;
    DgEngine::Graphics::Color mClubColorC = DgEngine::Graphics::Colors::Yellow;

    // Current animation indices
    int mCurrentAnimIndexA = 0;
    int mCurrentAnimIndexB = 0;

    // Timer / controls
    float mSceneTimer = 0.0f;
    float mSceneDuration = 45.0f;
    bool mPauseScene = false;
    bool mLoopScene = true;
    bool mDrawSkeleton = false;

    // Per-character speed differences
    float mAnimationSpeedA = 1.0f;
    float mAnimationSpeedB = 1.0f;

    bool mPendingDelayedAnimB = false;
    float mDelayedAnimBTimer = 0.0f;
    float mDelayedAnimBStartDelay = 2.5f;
    int mPendingAnimIndexB = -1;

    // Event listener 
    size_t mCycleListenerId = 0;
};