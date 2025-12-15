#include"GameState.h"

using namespace DgEngine;
using namespace DgEngine::Graphics;
using namespace DgEngine::Input;

void GameState::Initialize()
{
    mCamera.SetPosition({ 0.0f, 1.5f, -2.0f });
    mCamera.SetLookAt({ 0.0f, 1.0f, 0.0f });

    mDirectionalLight.direction = Math::Normalize({ 1.0f, -1.0f, 1.0f });
    mDirectionalLight.ambient = { 0.4f, 0.4f, 0.4f, 1.0f };
    mDirectionalLight.diffuse = { 0.8f, 0.8f, 0.8f, 1.0f };
    mDirectionalLight.specular = { 0.9f, 0.9f, 0.9f, 1.0f };

    mTerrain.Initialize(L"../../Assets/Textures/terrain/heightmap_1024x1024.raw", 20.0f);
    mGround.meshBuffer.Initialize(mTerrain.mesh);
    mGround.diffuseMapId = TextureManager::Get()->LoadTexture(L"../../Assets/Textures/terrain/dirt_seamless.jpg");
    mGround.specMapId = TextureManager::Get()->LoadTexture(L"../../Assets/Textures/terrain/grass_2048.jpg");

    mCharacter.Initialize("Character01/Character01.model");
    mCharacter02.Initialize("Character02/Character02.model");
    mCharacter03.Initialize("Character03/Character03.model");

	mCharacter.transform.position.z = 15;
	mCharacter02.transform.position.z = 15;
	mCharacter03.transform.position.z = 15; 
	mCharacter.transform.position.x = 19;
    mCharacter02.transform.position.x = 18;
    mCharacter03.transform.position.x = 20;

    MeshPX screenQuadMesh = MeshBuilder::CreateScreenQuadPX();
    mScreenQuad.meshBuffer.Initialize(screenQuadMesh);

    std::filesystem::path shaderFile = L"../../Assets/Shaders/Standard.fx";
    mStandardEffect.Initialize(shaderFile);
    mStandardEffect.SetCamera(mCamera);
    mStandardEffect.SetDirectionalLight(mDirectionalLight);
    mStandardEffect.SetLightCamera(mShadowEffect.GetLightCamera());
    mStandardEffect.SetShadowMap(mShadowEffect.GetDepthMap());

    shaderFile = L"../../Assets/Shaders/Shadow.fx";
    mShadowEffect.Initialize();
    mShadowEffect.SetDirectionalLight(mDirectionalLight);
	

    mTerrainEffect.Initialize();
    mTerrainEffect.SetCamera(mCamera);
    mTerrainEffect.SetLightCamera(mShadowEffect.GetLightCamera());
    mTerrainEffect.SetDirectionalLight(mDirectionalLight);
    mTerrainEffect.SetShadowMap(mShadowEffect.GetDepthMap());

    GraphicsSystem* gs = GraphicsSystem::Get();
    const uint32_t screenWidth = gs->GetBackBufferWidth();
    const uint32_t screenHeight = gs->GetBackBufferHeight();

    mSceneRenderTarget.Initialize(
        screenWidth,
        screenHeight,
        RenderTarget::Format::RGBA_U8);

    std::filesystem::path crtShaderFile = L"../../Assets/Shaders/CRT.fx";
    mCRTEffect.Initialize(crtShaderFile);
    mCRTEffect.SetScreenSize((float)screenWidth, (float)screenHeight);
    mCRTEffect.SetSourceTexture(mSceneRenderTarget);

    // Vision effect
    std::filesystem::path visionShaderFile = L"../../Assets/Shaders/Vision.fx";
    mVisionEffect.Initialize(visionShaderFile);
    mVisionEffect.SetScreenSize(static_cast<float>(screenWidth), static_cast<float>(screenHeight));
    mVisionEffect.SetSourceTexture(mSceneRenderTarget);
}


void GameState::Terminate()
{
    mTerrainEffect.Terminate();
	mShadowEffect.Terminate();
    mScreenQuad.Terminate();
    mCharacter03.Terminate();
    mCharacter02.Terminate();
    mCharacter.Terminate();
    mGround.Terminate();
    mStandardEffect.Terminate();
}


void GameState::Update(float deltaTime)
{
    mTime += deltaTime;
    mVisionEffect.SetTime(mTime);
    mCRTEffect.SetTime(mTime);

    UpdateCamera(deltaTime);
}

void GameState::Render()
{
    // 1) Render scene into offscreen texture
    mSceneRenderTarget.BeginRender();

    // Shadow pass
    mShadowEffect.Begin();
    mShadowEffect.Render(mCharacter);
    mShadowEffect.Render(mCharacter02);
    mShadowEffect.Render(mCharacter03);
    mShadowEffect.End();

    // Terrain
    mTerrainEffect.Begin();
    mTerrainEffect.Render(mGround);
    mTerrainEffect.End();

    // Standard 
    mStandardEffect.Begin();
    mStandardEffect.Render(mCharacter);
    mStandardEffect.Render(mCharacter02);
    mStandardEffect.Render(mCharacter03);
    mStandardEffect.End();

    mSceneRenderTarget.EndRender();

    // 2) Post-process pass: fullscreen quad with VisionEffect
    if (mPostProcessMode == 1)
    {
        mVisionEffect.Begin();
        mVisionEffect.Render(mScreenQuad);
        mVisionEffect.End();
    }
    else if (mPostProcessMode == 2)
    {
        mCRTEffect.Begin();
        mCRTEffect.Render(mScreenQuad);
        mCRTEffect.End();
    }
}




void GameState::DebugUI()
{
    ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (ImGui::DragFloat3("Direction#Light", &mDirectionalLight.direction.x, 0.01f))
        {
            mDirectionalLight.direction = Math::Normalize(mDirectionalLight.direction);
        }

        ImGui::ColorEdit4("Ambient#Light", &mDirectionalLight.ambient.r);
        ImGui::ColorEdit4("Diffuse#Light", &mDirectionalLight.diffuse.r);
        ImGui::ColorEdit4("Specular#Light", &mDirectionalLight.specular.r);
    }

    ImGui::Separator();

    if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
    {
        for (uint32_t i = 0; i < mCharacter.renderObjects.size(); ++i)
        {
            Material& material = mCharacter.renderObjects[i].material;
            std::string renderObjectId = "RenderObject " + std::to_string(i);
            ImGui::PushID(renderObjectId.c_str());
            if (ImGui::CollapsingHeader(renderObjectId.c_str()))
            {
                ImGui::LabelText("label", "Material:");
                ImGui::ColorEdit4("Emissive#Material", &material.emissive.r);
                ImGui::ColorEdit4("Ambient#Material", &material.ambient.r);
                ImGui::ColorEdit4("Diffuse#Material", &material.diffuse.r);
                ImGui::ColorEdit4("Specular#Material", &material.specular.r);
                ImGui::DragFloat("Shininess#Material", &material.shininess, 0.1f, 0.1f, 10000.0f);
            }
            ImGui::PopID();
        }
    }
    ImGui::DragFloat3("CharacterPosition", &mCharacter.transform.position.x, 0.1f);
    ImGui::DragFloat3("Character02Position", &mCharacter02.transform.position.x, 0.1f);
	ImGui::DragFloat3("Character03Position", &mCharacter03.transform.position.x, 0.1f);
	{
	    Math::Vector3 cameraPos = mCamera.GetPosition();
	    if (ImGui::DragFloat3("CameraPosition", &cameraPos.x, 0.1f))
	    {
	        mCamera.SetPosition(cameraPos);
	    }
	}
	ImGui::Separator();
    if (ImGui::CollapsingHeader("Post Process"))
    {
        const char* ppModes[] = { "None", "Vision", "CRT" };
        ImGui::Combo("Post Process Mode", &mPostProcessMode, ppModes, IM_ARRAYSIZE(ppModes));
    }

    mVisionEffect.DebugUI();
    mCRTEffect.DebugUI();
    ImGui::Separator();


    mStandardEffect.DebugUI();
    mShadowEffect.DebugUI();
    mTerrainEffect.DebugUI();

    ImGui::End();
}

void GameState::UpdateCamera(float deltaTime)
{
    InputSystem* input = InputSystem::Get();
    const float moveSpeed = input->IsKeyDown(KeyCode::LSHIFT) ? 10.0f : 4.0f;
    const float turnSpeed = 0.95f;

    if (input->IsKeyDown(KeyCode::W)) { mCamera.Walk(moveSpeed * deltaTime); }

    else if (input->IsKeyDown(KeyCode::S)) { mCamera.Walk(-moveSpeed * deltaTime); }

    else if (input->IsKeyDown(KeyCode::D)) { mCamera.Strafe(moveSpeed * deltaTime); }

    else if (input->IsKeyDown(KeyCode::A)) { mCamera.Strafe(-moveSpeed * deltaTime); }

    else if (input->IsKeyDown(KeyCode::E)) { mCamera.Rise(moveSpeed * deltaTime); }

    else if (input->IsKeyDown(KeyCode::Q)) { mCamera.Rise(-moveSpeed * deltaTime); }

    if (input->IsMouseDown(MouseButton::RBUTTON))
    {
        mCamera.Yaw(input->GetMouseMoveX() * turnSpeed * deltaTime);
        mCamera.Pitch(input->GetMouseMoveY() * turnSpeed * deltaTime);
    }

    Math::Vector3 cameraPosition = mCamera.GetPosition();
    float height = mTerrain.GetHeight(cameraPosition);
    if (height >= 0.0f)
    {
        cameraPosition.y = height + 2.0f;
        mCamera.SetPosition(cameraPosition);
    }
}

