#include "Precompiled.h"
#include "NetworkService.h"

using namespace DgEngine;
using namespace DgEngine::Network;

void NetworkService::Update(float deltaTime)
{
	NetworkManager::Get()->Update(deltaTime);
}
void NetworkService::DebugUI()
{
	NetworkManager::Get()->DebugUI();
}