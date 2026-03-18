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
    mCamera.SetPosition({ 0.0f, 2.8f, -6.5f });
    mCamera.SetLookAt({ 0.0f, 1.8f, 0.0f });

    // ------------------------------------------------------------
    // Light
    // ------------------------------------------------------------
    mDirectionalLight.direction = Math::Normalize({ 0.2f, -1.0f, 0.2f });
    mDirectionalLight.ambient = { 0.65f, 0.65f, 0.65f, 1.0f };
    mDirectionalLight.diffuse = { 1.2f, 1.2f, 1.2f, 1.0f };
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
        L"../../Assets/Models/James/Animations/JamesDancing3.animset",
        L"../../Assets/Models/James/Animations/JamesDancing4.animset",
        L"../../Assets/Models/James/Animations/JamesFalling.animset"
    };

    mCharacterBAnimSets =
    {
        L"../../Assets/Models/Sophia/Animations/SophiaDancing1.animset",
        L"../../Assets/Models/Sophia/Animations/SophiaDancing2.animset",
        L"../../Assets/Models/Sophia/Animations/SophiaDancing3.animset",
        L"../../Assets/Models/Sophia/Animations/SophiaDancing4.animset",
        L"../../Assets/Models/Sophia/Animations/SophiaArrow.animset"
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
    mCharacterA.transform.position = { -1.5f, 0.0f, 0.0f };
    mCharacterA.transform.rotation = Math::Quaternion::CreateFromAxisAngle(
        Math::Vector3::YAxis,
        -90.0f * Math::Constants::DegToRad);

    mCharacterB.transform.position = { 1.5f, 0.0f, 0.0f };
    mCharacterB.transform.rotation = Math::Quaternion::CreateFromAxisAngle(
        Math::Vector3::YAxis,
        90.0f * Math::Constants::DegToRad);

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
    mLightPulse += deltaTime;

    mDiscoBallRotation += deltaTime * 1.5f;

    // Update  animators
    mAnimatorA.Update(deltaTime * mAnimationSpeedA);
    mAnimatorB.Update(deltaTime * mAnimationSpeedB);

    // Extra sway motion
    const float bounceA = 0.08f + fabsf(sinf(mSceneTimer * 2.2f)) * 0.12f;
    const float bounceB = 0.08f + fabsf(sinf(mSceneTimer * 1.9f + 0.7f)) * 0.12f;

    for (auto& ro : mCharacterA.renderObjects)
    {
        ro.transform.position.y = bounceA;
    }

    for (auto& ro : mCharacterB.renderObjects)
    {
        ro.transform.position.y = bounceB;
    }
  
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

    if (mPendingDelayedAnimB)
    {
        mDelayedAnimBTimer += deltaTime;

        if (mDelayedAnimBTimer >= mDelayedAnimBStartDelay)
        {
            mAnimatorA.PlayAnimation(mPendingAnimIndexB, true);
            mPendingDelayedAnimB = false;
            mDelayedAnimBTimer = 0.0f;
            mPendingAnimIndexB = -1;
        }
    }
}

void GameState::Render()
{
    // Ground 
    DrawDiscoFloor();
    DrawDiscoBall();
    DrawClubLights();
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
    mAnimatorB.PlayAnimation(mCurrentAnimIndexB, true);

    // Delay only the last animation for Character B
    const int lastAnimIndexB = static_cast<int>(mCharacterAAnimSets.size()) - 1;

    if (mCurrentAnimIndexB == lastAnimIndexB)
    {
        mPendingDelayedAnimB = true;
        mDelayedAnimBTimer = 0.0f;
        mPendingAnimIndexB = mCurrentAnimIndexB;
    }
    else
    {
        mPendingDelayedAnimB = false;
        mDelayedAnimBTimer = 0.0f;
        mPendingAnimIndexB = -1;
        mAnimatorA.PlayAnimation(mCurrentAnimIndexB, true);
    }
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

void GameState::DrawDiscoFloor()
{
    const int halfCount = 5;
    const float tileSize = 1.0f;
    const float y = 0.01f;

    for (int z = -halfCount; z < halfCount; ++z)
    {
        for (int x = -halfCount; x < halfCount; ++x)
        {
            const float worldX = x * tileSize;
            const float worldZ = z * tileSize;

            const float pulse = sinf(mSceneTimer * 4.0f + (x + z) * 0.8f);
            const float pulse2 = sinf(mSceneTimer * 3.2f + (x - z) * 0.6f);

            Color color;
            if (((x + z) & 1) == 0)
            {
                color = { 0.5f + 0.5f * pulse, 0.2f, 0.8f + 0.2f * pulse2, 1.0f };
            }
            else
            {
                color = { 0.2f, 0.8f + 0.2f * pulse, 0.5f + 0.5f * pulse2, 1.0f };
            }

            Math::Vector3 p0{ worldX, y, worldZ };
            Math::Vector3 p1{ worldX + tileSize, y, worldZ };
            Math::Vector3 p2{ worldX + tileSize, y, worldZ + tileSize };
            Math::Vector3 p3{ worldX, y, worldZ + tileSize };

            SimpleDraw::AddLine(p0, p1, color);
            SimpleDraw::AddLine(p1, p2, color);
            SimpleDraw::AddLine(p2, p3, color);
            SimpleDraw::AddLine(p3, p0, color);
        }
    }
}

void GameState::DrawDiscoBall()
{
    const Math::Vector3 center = { 0.0f, 5.5f, 0.0f };
    const float radius = 0.65f;

    // Hanging string
    SimpleDraw::AddLine(
        { 0.0f, 6.9f, 0.0f },
        center,
        Colors::White);

    const int rings = 10;
    const int segments = 24;
    const int meridians = 18;

    const float t = mSceneTimer;

    // Soft glass-like colors
    Color glassA =
    {
        0.45f + 0.25f * sinf(t * 2.2f),
        0.65f + 0.20f * sinf(t * 1.7f + 1.2f),
        0.95f,
        1.0f
    };

    Color glassB =
    {
        0.75f + 0.20f * sinf(t * 1.9f + 2.1f),
        0.45f + 0.20f * sinf(t * 2.5f),
        0.95f,
        1.0f
    };

    Color highlight =
    {
        0.95f,
        0.95f,
        1.0f,
        1.0f
    };

    // Horizontal rings
    for (int r = 1; r < rings; ++r)
    {
        float v = (float)r / (float)rings;
        float phi = v * Math::Constants::Pi;

        float y = cosf(phi) * radius;
        float ringRadius = sinf(phi) * radius;

        Color ringColor = ((r % 2) == 0) ? glassA : glassB;

        // Slightly brighter near the upper half for a glass highlight feel
        if (y > 0.15f)
            ringColor = highlight;

        for (int i = 0; i < segments; ++i)
        {
            float a0 = (i / (float)segments) * Math::Constants::TwoPi;
            float a1 = ((i + 1) / (float)segments) * Math::Constants::TwoPi;

            Math::Vector3 p0 =
            {
                center.x + cosf(a0 + mDiscoBallRotation) * ringRadius,
                center.y + y,
                center.z + sinf(a0 + mDiscoBallRotation) * ringRadius
            };

            Math::Vector3 p1 =
            {
                center.x + cosf(a1 + mDiscoBallRotation) * ringRadius,
                center.y + y,
                center.z + sinf(a1 + mDiscoBallRotation) * ringRadius
            };

            SimpleDraw::AddLine(p0, p1, ringColor);
        }
    }

    // Vertical  lines
    for (int m = 0; m < meridians; ++m)
    {
        float theta = (m / (float)meridians) * Math::Constants::TwoPi + mDiscoBallRotation;

        Color meridianColor = ((m % 2) == 0) ? glassB : glassA;

        for (int j = 0; j < segments; ++j)
        {
            float phi0 = (j / (float)segments) * Math::Constants::Pi;
            float phi1 = ((j + 1) / (float)segments) * Math::Constants::Pi;

            Math::Vector3 p0 =
            {
                center.x + sinf(phi0) * cosf(theta) * radius,
                center.y + cosf(phi0) * radius,
                center.z + sinf(phi0) * sinf(theta) * radius
            };

            Math::Vector3 p1 =
            {
                center.x + sinf(phi1) * cosf(theta) * radius,
                center.y + cosf(phi1) * radius,
                center.z + sinf(phi1) * sinf(theta) * radius
            };

            SimpleDraw::AddLine(p0, p1, meridianColor);
        }
    }

    // Small top highlight arc
    for (int i = 0; i < 8; ++i)
    {
        float a0 = (i / 8.0f) * Math::Constants::Pi * 0.8f;
        float a1 = ((i + 1) / 8.0f) * Math::Constants::Pi * 0.8f;

        Math::Vector3 p0 =
        {
            center.x - 0.12f + cosf(a0) * 0.18f,
            center.y + 0.20f + sinf(a0) * 0.10f,
            center.z + radius * 0.78f
        };

        Math::Vector3 p1 =
        {
            center.x - 0.12f + cosf(a1) * 0.18f,
            center.y + 0.20f + sinf(a1) * 0.10f,
            center.z + radius * 0.78f
        };

        SimpleDraw::AddLine(p0, p1, highlight);
    }
}

void GameState::DrawClubLights()
{
    const Math::Vector3 leftRig = { -4.5f, 5.8f, -1.5f };
    const Math::Vector3 rightRig = { 4.5f, 5.8f, -1.5f };
    const Math::Vector3 centerRig = { 0.0f, 6.2f, -2.0f };

    const float sweepA = sinf(mLightPulse * 1.7f) * 3.0f;
    const float sweepB = cosf(mLightPulse * 1.3f) * 3.0f;
    const float sweepC = sinf(mLightPulse * 2.1f + 1.2f) * 2.5f;

    Math::Vector3 targetA = { -3.5f + sweepA, 0.15f, 0.0f };
    Math::Vector3 targetB = { 3.5f + sweepB, 0.15f, 0.0f };
    Math::Vector3 targetC = { 0.0f + sweepC, 0.15f, 1.5f };

    for (int i = 0; i < 4; ++i)
    {
        float offset = (float)i * 0.12f;

        SimpleDraw::AddLine(
            { leftRig.x + offset, leftRig.y, leftRig.z },
            { targetA.x, targetA.y, targetA.z + offset },
            mClubColorA);

        SimpleDraw::AddLine(
            { rightRig.x - offset, rightRig.y, rightRig.z },
            { targetB.x, targetB.y, targetB.z - offset },
            mClubColorB);

        SimpleDraw::AddLine(
            { centerRig.x, centerRig.y + offset, centerRig.z },
            { targetC.x + offset, targetC.y, targetC.z },
            mClubColorC);
    }
}