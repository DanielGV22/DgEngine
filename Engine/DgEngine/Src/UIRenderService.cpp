#include "Precompiled.h"
#include "UIRenderService.h"
#include "UIComponent.h"


using namespace DgEngine;
using namespace DgEngine::Graphics;

void UIRenderService::Terminate()
{
	mUIComponents.clear();
}
void UIRenderService::Render()
{
	UISpriteRenderer::Get()->BeginRender();
	for (UIComponent* uiComponent : mUIComponents)
	{
		uiComponent->Render();
	}
	UISpriteRenderer::Get()->EndRender();
}

void UIRenderService::Serialize(rapidjson::Document& doc, rapidjson::Value& value)
{
	rapidjson::Value serviceValue(rapidjson::kObjectType);
	value.AddMember("UIRenderService", serviceValue, doc.GetAllocator());
}

void UIRenderService::Register(UIComponent* uiComponent)
{
	auto iter = std::find(mUIComponents.begin(), mUIComponents.end(), uiComponent);
	if (iter == mUIComponents.end())
	{
		mUIComponents.push_back(uiComponent);
	}
}
void UIRenderService::Unregister(UIComponent* uiComponent)
{
	auto iter = std::find(mUIComponents.begin(), mUIComponents.end(), uiComponent);
	if (iter != mUIComponents.end())
	{
		mUIComponents.erase(iter);
	}
}