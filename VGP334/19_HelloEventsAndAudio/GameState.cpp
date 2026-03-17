#include"GameState.h"
#include"GameEvents.h"

using namespace DgEngine;
using namespace DgEngine::Graphics;
using namespace DgEngine::Input;
using namespace DgEngine::Physics;
using namespace DgEngine::Audio;

void GameState::Initialize()
{
	mCamera.SetPosition({ 0.0f, 1.0f, -3.0f });
	mCamera.SetLookAt({ 0.0f, 1.0f, 0.0f });

	mParticleSystemEffect.Initialize();
	mParticleSystemEffect.SetCamera(mCamera);

	ParticleSystemInfo info;
	info.textureId = TextureManager::Get()->LoadTexture("Images/bullet2.png");
	info.maxParticles = 1000;
	info.particlesPerEmit = { 8, 12 };
	info.delay = 1.0f;
	info.lifeTime = FLT_MAX;
	info.timeBetweenEmits = { 0.1f, 0.2f };
	info.spawnAngle = { -30.0f, 30.0f };
	info.spawnSpeed = { 1.0f, 3.0f };
	info.particleLifeTime = { 0.5f, 2.0f };
	info.spawnDirection = Math::Vector3::YAxis;
	info.spawnPosition = Math::Vector3::Zero;
	info.startScale = { Math::Vector3::One, Math::Vector3::One };
	info.endScale = { Math::Vector3::Zero, Math::Vector3::Zero };
	info.startColor = { Graphics::Colors::White, Graphics::Colors::White };
	info.endColor = { Graphics::Colors::White, Graphics::Colors::White };
	mParticleSystem.Initialize(info);

	info.textureId = TextureManager::Get()->LoadTexture("Images/mushroom.png");
	info.particlesPerEmit = { 10, 30 };
	info.spawnSpeed = { 7.0f, 15.0f };
	info.delay = 0.0f;
	info.lifeTime = 0.0f;
	info.spawnAngle = { -180.0f, 180.0f };
	info.spawnDirection = Math::Vector3::YAxis;
	info.startColor = { Graphics::Colors::Red, Graphics::Colors::Yellow };
	info.endColor = { Graphics::Colors::LightGray, Graphics::Colors::White };
	mFireworkParticles.Initialize(info);

	EventManager* em = EventManager::Get();
	mSpacePressedListenerId = em->AddListener(PressSpaceEvent::StaticGetTypeId(),
		std::bind(&GameState::OnSpacePressedEvent, this, std::placeholders::_1));
	mEnterPressedListenerId = em->AddListener(PressEnterEvent::StaticGetTypeId(),
		[](const Core::Event& e)
		{
			LOG("Enter was pressed");
		});

	SoundEffectManager* sm = SoundEffectManager::Get();
	mLaunchSoundId = sm->Load("megamanx_blast.wav");
	mExplosionSoundId = sm->Load("explosion.wav");

	auto launchFirework = [&]()
		{
			SoundEffectManager::Get()->Play(mLaunchSoundId);
		};
	auto spawnFireworks = [&]()
		{
			mFireworkParticles.SpawnParticles();
			SoundEffectManager::Get()->Play(mExplosionSoundId);
		};
	mFireworkAnimation = AnimationBuilder()
		.AddPositionKey(Math::Vector3::Zero, 0.0f)
		.AddPositionKey(Math::Vector3::Zero, 2.0f)
		.AddPositionKey(Math::Vector3::YAxis * 5.0f, 3.0f)
		.AddEventKey(launchFirework, 2.0f)
		.AddEventKey(spawnFireworks, 3.0f)
		.Build();
}

void GameState::Terminate()
{
	SoundEffectManager* sm = SoundEffectManager::Get();
	sm->Stop(mLaunchSoundId);
	sm->Stop(mExplosionSoundId);

	EventManager* em = EventManager::Get();
	em->RemoveListener(PressSpaceEvent::StaticGetTypeId(), mSpacePressedListenerId);
	em->RemoveListener(PressEnterEvent::StaticGetTypeId(), mEnterPressedListenerId);
	mFireworkParticles.Terminate();
	mParticleSystem.Terminate();
	mParticleSystemEffect.Terminate();
}

void GameState::Update(float deltaTime)
{
	UpdateCamera(deltaTime);
	mFireworkParticles.Update(deltaTime);
	mParticleSystem.Update(deltaTime);

	float prevTime = mFireworkAnimationTime;
	mFireworkAnimationTime += deltaTime;
	if (mFireworkAnimation.GetDuration() > 0)
	{
		mFireworkAnimation.PlayEvents(prevTime, mFireworkAnimationTime);
		while (mFireworkAnimationTime >= mFireworkAnimation.GetDuration())
		{
			mFireworkAnimationTime -= mFireworkAnimation.GetDuration();
		}
	}
}

void GameState::Render()
{
	SimpleDraw::AddGroundPlane(10.0f, Colors::White);
	SimpleDraw::Render(mCamera);

	Transform particleTransform = mFireworkAnimation.GetTransform(mFireworkAnimationTime);
	mParticleSystem.SetPosition(particleTransform.position);
	mFireworkParticles.SetPosition(particleTransform.position);
	mParticleSystemEffect.Begin();
	mParticleSystem.Render(mParticleSystemEffect);
	mFireworkParticles.Render(mParticleSystemEffect);
	mParticleSystemEffect.End();
}

void GameState::DebugUI()
{
	ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	mParticleSystem.DebugUI();
	PhysicsWorld::Get()->DebugUI();
	ImGui::End();
}

void GameState::UpdateCamera(float deltaTime)
{
	Input::InputSystem* input = Input::InputSystem::Get();
	const float moveSpeed = input->IsKeyDown(Input::KeyCode::LSHIFT) ? 10.0f : 1.0f;
	const float turnSpeed = 1.0f;

	if (input->IsKeyDown(Input::KeyCode::W)) mCamera.Walk(moveSpeed * deltaTime);
	else if (input->IsKeyDown(Input::KeyCode::S)) mCamera.Walk(-moveSpeed * deltaTime);

	if (input->IsKeyDown(Input::KeyCode::A)) mCamera.Strafe(-moveSpeed * deltaTime);
	else if (input->IsKeyDown(Input::KeyCode::D)) mCamera.Strafe(moveSpeed * deltaTime);

	if (input->IsKeyDown(Input::KeyCode::Q)) mCamera.Rise(-moveSpeed * deltaTime);
	else if (input->IsKeyDown(Input::KeyCode::E)) mCamera.Rise(moveSpeed * deltaTime);

	if (input->IsMouseDown(MouseButton::RBUTTON))
	{
		mCamera.Yaw(input->GetMouseMoveX() * turnSpeed * deltaTime);
		mCamera.Pitch(input->GetMouseMoveY() * turnSpeed * deltaTime);
	}

	if (input->IsKeyPressed(KeyCode::SPACE))
	{
		PressSpaceEvent event;
		EventManager::Broadcast(event);
	}
	if (input->IsKeyPressed(KeyCode::ENTER))
	{
		PressEnterEvent event;
		EventManager::Broadcast(event);
	}
}

void GameState::OnSpacePressedEvent(const DgEngine::Core::Event& e)
{
	LOG("Space was pressed");
	mFireworkParticles.SpawnParticles();
}