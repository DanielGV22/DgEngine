#include "GameState.h"

using namespace DgEngine;
using namespace DgEngine::Graphics;
using namespace DgEngine::Input;
using namespace DgEngine::Physics;
using namespace DgEngine::Audio;

void GameState::Initialize()
{
    // ------------------------------------------------------------
    // Camera
    // ------------------------------------------------------------
    mCamera.SetPosition({ 0.0f, 2.8f, -11.0f });
    mCamera.SetLookAt({ 0.0f, 1.8f, 0.0f });

    // ------------------------------------------------------------
    // Light
    // ------------------------------------------------------------
    mDirectionalLight.direction = Math::Normalize({ 1.0f, -1.0f, 1.0f });
    mDirectionalLight.ambient = { 0.35f, 0.35f, 0.35f, 1.0f };
    mDirectionalLight.diffuse = { 0.85f, 0.85f, 0.85f, 1.0f };
    mDirectionalLight.specular = { 1.0f, 1.0f, 1.0f, 1.0f };

    std::filesystem::path shaderFile = L"../../Assets/Shaders/Standard.fx";
    mStandardEffect.Initialize(shaderFile);
    mStandardEffect.SetCamera(mCamera);
    mStandardEffect.SetDirectionalLight(mDirectionalLight);

    // ------------------------------------------------------------
    // Animation files list
    // ------------------------------------------------------------
    mCharacterAAnimSets =
    {
        L"../../Assets/Models/James/Animations/JamesDancing1.animset",
        L"../../Assets/Models/James/Animations/JamesDancing2.animset",
        L"../../Assets/Models/James/Animations/JamesDancing3.animset"
    };

    mCharacterBAnimSets =
    {
        L"../../Assets/Models/Sophia/Animations/SophiaDancing1.animset",
        L"../../Assets/Models/Sophia/Animations/SophiaDancing2.animset",
        L"../../Assets/Models/Sophia/Animations/SophiaDancing3.animset"
    };

    // ------------------------------------------------------------
    // Models + animators
    // ------------------------------------------------------------
    auto* modelManager = ModelManager::Get();

    // Character A
    mCharacterA.Initialize("James/James.model");
    mCharacterA.animator = &mAnimatorA;

    for (const auto& anim : mCharacterAAnimSets)
        modelManager->AddAnimation(mCharacterA.modelId, anim);

    mAnimatorA.Initialize(mCharacterA.modelId);

    // Character B
    mCharacterB.Initialize("Sophia/Sophia.model");
    mCharacterB.animator = &mAnimatorB;

    for (const auto& anim : mCharacterBAnimSets)
        modelManager->AddAnimation(mCharacterB.modelId, anim);

    mAnimatorB.Initialize(mCharacterB.modelId);

    // Position characters
    for (auto& ro : mCharacterA.renderObjects)
    {
        ro.transform.position = { -5.0f, 0.0f, 0.0f };
        ro.transform.rotation = Math::Quaternion::CreateFromAxisAngle(
            Math::Vector3::YAxis,
            15.0f * Math::Constants::DegToRad);
    }

    for (auto& ro : mCharacterB.renderObjects)
    {
        ro.transform.position = { 5.0f, 0.0f, 0.0f };
        ro.transform.rotation = Math::Quaternion::CreateFromAxisAngle(
            Math::Vector3::YAxis,
            -15.0f * Math::Constants::DegToRad);
    }

    // ------------------------------------------------------------
    // Particle systems
    // ------------------------------------------------------------
    mParticleSystemEffect.Initialize();
    mParticleSystemEffect.SetCamera(mCamera);

    ParticleSystemInfo particleInfo;
    particleInfo.textureId = TextureManager::Get()->LoadTexture("Images/mushroom.png");
    particleInfo.maxParticles = 300;
    particleInfo.particlesPerEmit = { 20, 30 };

    particleInfo.lifeTime = 0.15f;              // emitter only lives briefly
    particleInfo.timeBetweenEmits = { 0.01f, 0.02f };

    particleInfo.delay = 0.0f;
    particleInfo.spawnAngle = { -180.0f, 180.0f };
    particleInfo.spawnSpeed = { 2.0f, 5.0f };
    particleInfo.particleLifeTime = { 0.35f, 0.8f };
    particleInfo.spawnDirection = Math::Vector3::YAxis;
    particleInfo.spawnPosition = Math::Vector3::Zero;
    particleInfo.startScale = { Math::Vector3::One * 0.15f, Math::Vector3::One * 0.30f };
    particleInfo.endScale = { Math::Vector3::Zero, Math::Vector3::One * 0.05f };
    particleInfo.startColor = { Colors::Yellow, Colors::Orange };
    particleInfo.endColor = { Colors::Red, Colors::Transparent };

    mCenterParticles.Initialize(particleInfo);

    particleInfo.startColor = { Colors::Cyan, Colors::Blue };
    particleInfo.endColor = { Colors::White, Colors::Transparent };
    mLeftParticles.Initialize(particleInfo);

    particleInfo.startColor = { Colors::Magenta, Colors::HotPink };
    particleInfo.endColor = { Colors::White, Colors::Transparent };
    mRightParticles.Initialize(particleInfo);

    // ------------------------------------------------------------
    // Audio
    // ------------------------------------------------------------
    auto* soundManager = SoundEffectManager::Get();

    mMusicTracks =
    {
        soundManager->Load("FeelslikeSummer.wav"),
        soundManager->Load("AmericanMastiff.wav")
    };

    mBurstSoundId = soundManager->Load("explosion.wav");

    // ------------------------------------------------------------
    // Events
    // ------------------------------------------------------------
    RegisterEvents();

    // ------------------------------------------------------------
    // Initial playback state
    // ------------------------------------------------------------
    mCurrentAnimIndexA = 0;
    mCurrentAnimIndexB = 0;

    ApplyCurrentAnimationSet();
    PlayCurrentMusic();
    ResetScene();
}

void GameState::Terminate()
{
    UnregisterEvents();

    auto* soundManager = SoundEffectManager::Get();

    if (mCurrentMusic != 0)
        soundManager->Stop(mCurrentMusic);

    if (mBurstSoundId != 0)
        soundManager->Stop(mBurstSoundId);

    mCenterParticles.Terminate();
    mLeftParticles.Terminate();
    mRightParticles.Terminate();
    mParticleSystemEffect.Terminate();

    mCharacterA.Terminate();
    mCharacterB.Terminate();

    mStandardEffect.Terminate();
}

void GameState::Update(float deltaTime)
{
    UpdateCamera(deltaTime);

    auto* input = InputSystem::Get();

    if (input->IsKeyPressed(KeyCode::SPACE))
        mPauseScene = !mPauseScene;

    if (input->IsKeyPressed(KeyCode::R))
        ResetScene();

    // ------------------------------------------------------------
    // Arrow keys for event
    // LEFT / DOWN = previous
    // RIGHT / UP  = next
    // ------------------------------------------------------------
    if (input->IsKeyPressed(KeyCode::LEFT) || input->IsKeyPressed(KeyCode::DOWN))
    {
        CycleAnimationEvent ev(CycleDirection::Previous);
        EventManager::Broadcast(ev);
    }

    if (input->IsKeyPressed(KeyCode::RIGHT) || input->IsKeyPressed(KeyCode::UP))
    {
        CycleAnimationEvent ev(CycleDirection::Next);
        EventManager::Broadcast(ev);
    }

    if (mPauseScene)
        return;

    mSceneTimer += deltaTime;

    // Update  animators
    mAnimatorA.Update(deltaTime * mAnimationSpeedA);
    mAnimatorB.Update(deltaTime * mAnimationSpeedB);

    // Extra sway motion
    const float bounceA = 0.08f + fabsf(sinf(mSceneTimer * 2.2f)) * 0.22f;
    const float bounceB = 0.08f + fabsf(sinf(mSceneTimer * 1.9f + 0.7f)) * 0.22f;

  

    // Update particle locations
    mCenterParticles.SetPosition({ 0.0f, 2.5f, 0.0f });
    mLeftParticles.SetPosition({ -2.5f, 2.5f, 0.0f });
    mRightParticles.SetPosition({ 2.5f, 2.5f, 0.0f });

    mCenterParticles.Update(deltaTime);
    mLeftParticles.Update(deltaTime);
    mRightParticles.Update(deltaTime);

    if (mSceneTimer >= mSceneDuration)
    {
        if (mLoopScene)
            ResetScene();
        else
            mPauseScene = true;
    }
}

void GameState::Render()
{
    // Ground 
    SimpleDraw::AddGroundPlane(20.0f, Colors::DarkGray);
    SimpleDraw::AddLine({ -6.0f, 0.0f, 0.0f }, { 6.0f, 0.0f, 0.0f }, Colors::White);
    SimpleDraw::Render(mCamera);

    // Characters
    mStandardEffect.Begin();
    mStandardEffect.Render(mCharacterA);
    mStandardEffect.Render(mCharacterB);
    mStandardEffect.End();

    // Particles
    mParticleSystemEffect.Begin();
    mCenterParticles.Render(mParticleSystemEffect);
    mLeftParticles.Render(mParticleSystemEffect);
    mRightParticles.Render(mParticleSystemEffect);
    mParticleSystemEffect.End();

    // Skeleton debug
    if (mDrawSkeleton)
    {
        AnimationUtil::BoneTransforms boneTransformsA;
        AnimationUtil::BoneTransforms boneTransformsB;

        AnimationUtil::ComputeBoneTransforms(mCharacterA.modelId, boneTransformsA, &mAnimatorA);
        AnimationUtil::ComputeBoneTransforms(mCharacterB.modelId, boneTransformsB, &mAnimatorB);

        AnimationUtil::DrawSkeleton(mCharacterA.modelId, boneTransformsA);
        AnimationUtil::DrawSkeleton(mCharacterB.modelId, boneTransformsB);
    }
}

void GameState::DebugUI()
{
    ImGui::Begin("Final Animated Scene", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Text("Scene Timer: %.2f / %.2f", mSceneTimer, mSceneDuration);
    ImGui::ProgressBar(mSceneTimer / mSceneDuration, ImVec2(260.0f, 0.0f));

    ImGui::Separator();
    ImGui::Text("Arrow Keys: cycle animations + change music + spawn particles");
    ImGui::Text("SPACE: pause");
    ImGui::Text("R: reset");

    ImGui::Separator();
    ImGui::Text("Character A Index: %d", mCurrentAnimIndexA);
    ImGui::Text("Character B Index: %d", mCurrentAnimIndexB);

    if (!mCharacterAAnimSets.empty())
        ImGui::Text("A Anim: %ls", mCharacterAAnimSets[mCurrentAnimIndexA].c_str());

    if (!mCharacterBAnimSets.empty())
        ImGui::Text("B Anim: %ls", mCharacterBAnimSets[mCurrentAnimIndexB].c_str());

    ImGui::Separator();
    ImGui::Checkbox("Pause Scene", &mPauseScene);
    ImGui::Checkbox("Loop Scene", &mLoopScene);
    ImGui::Checkbox("Draw Skeleton", &mDrawSkeleton);

    ImGui::DragFloat("Anim Speed A", &mAnimationSpeedA, 0.01f, 0.1f, 3.0f);
    ImGui::DragFloat("Anim Speed B", &mAnimationSpeedB, 0.01f, 0.1f, 3.0f);

    if (ImGui::Button("Previous Animation"))
    {
        CycleAnimationEvent ev(CycleDirection::Previous);
        EventManager::Broadcast(ev);
    }

    ImGui::SameLine();

    if (ImGui::Button("Next Animation"))
    {
        CycleAnimationEvent ev(CycleDirection::Next);
        EventManager::Broadcast(ev);
    }

    if (ImGui::Button("Reset Scene"))
        ResetScene();

    ImGui::Separator();
    mStandardEffect.DebugUI();

    ImGui::End();
}

void GameState::UpdateCamera(float deltaTime)
{
    auto* input = InputSystem::Get();
    const float moveSpeed = input->IsKeyDown(KeyCode::LSHIFT) ? 10.0f : 3.0f;
    const float turnSpeed = 0.1f;

    if (input->IsKeyDown(KeyCode::W))
        mCamera.Walk(moveSpeed * deltaTime);
    if (input->IsKeyDown(KeyCode::S))
        mCamera.Walk(-moveSpeed * deltaTime);
    if (input->IsKeyDown(KeyCode::A))
        mCamera.Strafe(-moveSpeed * deltaTime);
    if (input->IsKeyDown(KeyCode::D))
        mCamera.Strafe(moveSpeed * deltaTime);
    if (input->IsKeyDown(KeyCode::Q))
        mCamera.Rise(-moveSpeed * deltaTime);
    if (input->IsKeyDown(KeyCode::E))
        mCamera.Rise(moveSpeed * deltaTime);

    if (input->IsMouseDown(MouseButton::RBUTTON))
    {
        mCamera.Yaw(input->GetMouseMoveX() * turnSpeed * deltaTime);
        mCamera.Pitch(input->GetMouseMoveY() * turnSpeed * deltaTime);
    }
}

void GameState::RegisterEvents()
{
    auto* eventManager = EventManager::Get();

    mCycleListenerId = eventManager->AddListener(
        CycleAnimationEvent::StaticGetTypeId(),
        std::bind(&GameState::OnCycleAnimationEvent, this, std::placeholders::_1));
}

void GameState::UnregisterEvents()
{
    auto* eventManager = EventManager::Get();

    if (mCycleListenerId != 0)
    {
        eventManager->RemoveListener(CycleAnimationEvent::StaticGetTypeId(), mCycleListenerId);
        mCycleListenerId = 0;
    }
}

void GameState::OnCycleAnimationEvent(const DgEngine::Core::Event& e)
{
    const auto& ev = static_cast<const CycleAnimationEvent&>(e);

    const int countA = static_cast<int>(mCharacterAAnimSets.size());
    const int countB = static_cast<int>(mCharacterBAnimSets.size());

    if (countA == 0 || countB == 0)
        return;

    if (ev.direction == CycleDirection::Next)
    {
        mCurrentAnimIndexA = (mCurrentAnimIndexA + 1) % countA;
        mCurrentAnimIndexB = (mCurrentAnimIndexB + 1) % countB;
    }
    else
    {
        mCurrentAnimIndexA = (mCurrentAnimIndexA - 1 + countA) % countA;
        mCurrentAnimIndexB = (mCurrentAnimIndexB - 1 + countB) % countB;
    }

    ApplyCurrentAnimationSet();
    PlayCurrentMusic();
    TriggerCycleFX();
}

void GameState::ApplyCurrentAnimationSet()
{
    mAnimatorA.PlayAnimation(mCurrentAnimIndexA, true);
    mAnimatorB.PlayAnimation(mCurrentAnimIndexB, true);
}

void GameState::PlayCurrentMusic()
{
    auto* soundManager = SoundEffectManager::Get();

    if (mCurrentMusic != 0)
        soundManager->Stop(mCurrentMusic);

    // One music track per cycle
    if (!mMusicTracks.empty())
    {
        const int musicIndex = mCurrentAnimIndexA % static_cast<int>(mMusicTracks.size());
        mCurrentMusic = mMusicTracks[musicIndex];
        soundManager->Play(mCurrentMusic);
    }
}

void GameState::TriggerCycleFX()
{
    mCenterParticles.SpawnParticles();
    mLeftParticles.SpawnParticles();
    mRightParticles.SpawnParticles();

    if (mBurstSoundId != 0)
        SoundEffectManager::Get()->Play(mBurstSoundId);
}

void GameState::ResetScene()
{
    mSceneTimer = 0.0f;
    mPauseScene = false;

    ApplyCurrentAnimationSet();
}