#include"GameState.h"

using namespace DgEngine;
using namespace DgEngine::Graphics;
using namespace DgEngine::Input;

void GameState::Initialize()
{
    mCamera.SetPosition({ 0.0f, 1.0f, -6.0f });
    mCamera.SetLookAt({ 0.0f, 0.0f, 0.0f });
  
    mDirectionalLight.direction = Math::Normalize({ 1.0f, -1.0f, 1.0f });
    mDirectionalLight.ambient = { 0.4f, 0.4f, 0.4f, 1.0f };
    mDirectionalLight.diffuse = { 0.8f, 0.8f, 0.8f, 1.0f };
    mDirectionalLight.specular = { 1.0f, 1.0f, 1.0f, 1.0f };

	ModelManager* mm = ModelManager::Get();
    mCharacter.Initialize("Character03/Character03.model");
    mCharacter.animator = &mAnimator;
	mm->AddAnimation(mCharacter.modelId, L"../../Assets/Models/Character03/Animations/RumbaDancing.animset");
    

	mAnimator.Initialize(mCharacter.modelId);

    // Add this debug check
    if (mCharacter.skeleton == nullptr)
    {
        LOG("WARNING: Character skeleton is null!");
    }

    auto tm = TextureManager::Get();

    std::filesystem::path shaderFile = L"../../Assets/Shaders/Standard.fx";
    mStandardEffect.Initialize(shaderFile);
    mStandardEffect.SetCamera(mCamera);
    mStandardEffect.SetDirectionalLight(mDirectionalLight);
}


void GameState::Terminate()
{
    mCharacter.Terminate();
    mStandardEffect.Terminate();
}


void GameState::Update(float deltaTime)
{
    UpdateCamera(deltaTime);

	mAnimator.Update(deltaTime * mAnimationSpeed);
}

void GameState::Render()
{
   
    if (mDrawSkeleton)
    {
		AnimationUtil::BoneTransforms boneTransforms;
		AnimationUtil::ComputeBoneTransforms(mCharacter.modelId, boneTransforms, &mAnimator);
		AnimationUtil::DrawSkeleton(mCharacter.modelId, boneTransforms);
        SimpleDraw::AddGroundPlane(10.0f, Colors::DarkGray);
        SimpleDraw::Render(mCamera);
    }
    else
    {
        SimpleDraw::AddGroundPlane(10.0f, Colors::DarkGray);
        SimpleDraw::Render(mCamera);

        mStandardEffect.Begin();
        mStandardEffect.Render(mCharacter);
        mStandardEffect.End();
    }
}

void GameState::UpdateCamera(float deltaTime)
{
    Input::InputSystem* input = Input::InputSystem::Get();
    const float moveSpeed = input->IsKeyDown(KeyCode::LSHIFT) ? 10.0f : 1.0f;
    const float turnSpeed = 0.1f;

    if (input->IsKeyDown(KeyCode::W)) mCamera.Walk(moveSpeed * deltaTime);
    if (input->IsKeyDown(KeyCode::S)) mCamera.Walk(-moveSpeed * deltaTime);
    if (input->IsKeyDown(KeyCode::D)) mCamera.Strafe(moveSpeed * deltaTime);
    if (input->IsKeyDown(KeyCode::A)) mCamera.Strafe(-moveSpeed * deltaTime);
    if (input->IsKeyDown(KeyCode::E)) mCamera.Rise(moveSpeed * deltaTime);
    if (input->IsKeyDown(KeyCode::Q)) mCamera.Rise(-moveSpeed * deltaTime);

    if (input->IsMouseDown(MouseButton::RBUTTON))
    {
        mCamera.Yaw(input->GetMouseMoveX() * turnSpeed * deltaTime);
        mCamera.Pitch(input->GetMouseMoveY() * turnSpeed * deltaTime);
    }
}



void GameState::DebugUI()
{
    ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (ImGui::DragFloat3("Direction#Light", &mDirectionalLight.direction.x, 0.01f))
            mDirectionalLight.direction = Math::Normalize(mDirectionalLight.direction);

        ImGui::ColorEdit4("Ambient#Light", &mDirectionalLight.ambient.r);
        ImGui::ColorEdit4("Diffuse#Light", &mDirectionalLight.diffuse.r);
        ImGui::ColorEdit4("Specular#Light", &mDirectionalLight.specular.r);
    }

    if (ImGui::CollapsingHeader("Material - Character01", ImGuiTreeNodeFlags_DefaultOpen))
    {
        for (uint32_t i = 0; i < mCharacter.renderObjects.size(); ++i)
        {
            Material& material = mCharacter.renderObjects[i].material;
            std::string renderObjectId = "RenderObject01 " + std::to_string(i);
            ImGui::PushID(renderObjectId.c_str());
            if (ImGui::CollapsingHeader(renderObjectId.c_str()))
            {
                ImGui::ColorEdit4("Emissive#Material", &material.emissive.r);
                ImGui::ColorEdit4("Ambient#Material", &material.ambient.r);
                ImGui::ColorEdit4("Diffuse#Material", &material.diffuse.r);
                ImGui::ColorEdit4("Specular#Material", &material.specular.r);
                ImGui::DragFloat("Shininess#Material", &material.shininess, 0.1f, 0.1f, 100.0f);
            }
            ImGui::PopID();
        }
    }

	ImGui::Checkbox("Draw Skeleton", &mDrawSkeleton);
	ImGui::DragFloat("Animation Speed", &mAnimationSpeed, 0.1f, 0.1f, 10.0f);
	int maxAnimations = mAnimator.GetAnimationCount();
	if (ImGui::DragInt("AnimIndex", &mClipIndex, 1, -1, maxAnimations - 1))
    {
        mAnimator.PlayAnimation(mClipIndex, true);
    }

    mStandardEffect.DebugUI();
    ImGui::End();
}


