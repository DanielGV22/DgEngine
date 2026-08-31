#include "Precompiled.h"
#include "NetworkManager.h"

#include "Client.h"
#include "Server.h"

using namespace DgEngine;
using namespace DgEngine::Network;
using namespace DgEngine::Graphics;

namespace
{
	std::unique_ptr<NetworkManager> sNetworkManager;
	Core::WindowMessageHandler sWindowsMessageHandler;
}

LRESULT CALLBACK NetworkManager::NetworkManagerMessageHandler(HWND window, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (!sNetworkManager)
	{
		switch (msg)
		{
		case WM_SOCKET:
		{
			// check for error
			if (WSAGETSELECTERROR(lParam))
			{
				ASSERT(false, "Socket failed with error");
				sNetworkManager->Terminate();
			}
			else
			{
				switch (WSAGETSELECTEVENT(lParam))
				{
				case FD_READ:
				{
					sNetworkManager->ReceiveMsg();
					break;
				}
				}
			}
		}
		}
	}
	return sWindowsMessageHandler.ForwardMessage(window, msg, wParam, lParam);
}

void NetworkManager::StaticInitialize(HWND handle)
{
	ASSERT(sNetworkManager == nullptr, "NetworkManager already initialized!");
	sNetworkManager = std::make_unique<NetworkManager>();
	sNetworkManager->Initialize(handle);
}
void NetworkManager::StaticTerminate()
{
	if (sNetworkManager != nullptr)
	{
		sNetworkManager->Terminate();
		sNetworkManager.reset();
	}
}
NetworkManager* NetworkManager::Get()
{
	return sNetworkManager.get();
}

void NetworkManager::Initialize(HWND handle)
{
	mWindow = handle;
	mStarted = false;
	mServerAddress = "127.0.0.1"; // default local address for network on same device
}
void NetworkManager::StartNetwork(bool server, const std::string& serverAddress)
{
	mServer = server;
	if (server)
	{
		mNetwork = new Server(mPort);
		mNetwork->Initialize(mWindow, "");
	}
	else
	{
		mNetwork = new Client(mPort);
		mNetwork->Initialize(mWindow, serverAddress);

		std::string msg = "Client:connected";
		SendMsg(msg.c_str(), msg.size());
	}

	mConnected = false;
	mStarted = true;

	sWindowsMessageHandler.Hook(mWindow, NetworkManagerMessageHandler);
}
void NetworkManager::Update(float deltaTime)
{
	// if not started, return
	if (!mStarted)
	{
		return;
	}

	// if no data, return
	if (mNetwork->GetDataLength() <= 0)
	{
		return;
	}

	const char* data = mNetwork->GetData();
	LOG("[Network] %s", data);
	mConnected = true;

	// TODO handle all the messages that come and apply effects

	mNetwork->ResetMsg();
}
void NetworkManager::Terminate()
{
	sWindowsMessageHandler.Unhook();
	if (mNetwork != nullptr)
	{
		mNetwork->Terminate();
		delete mNetwork;
		mNetwork = nullptr;
	}
	mStarted = false;
	mConnected = false;
}
void NetworkManager::DebugUI()
{
	if (ImGui::CollapsingHeader("Network", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (mConnected)
		{
			if (mServer)
			{
				ImGui::LabelText("Element", "Server");
			}
			else
			{
				ImGui::LabelText("Element", "Client");
			}
			ImGui::LabelText("ServerAddress", mServerAddress.c_str());
			ImGui::LabelText("Port", "%d", mPort);
		}
		else if (mNetwork == nullptr)
		{
			ImGui::Checkbox("Server", &mServer);
			int port = mPort;
			if (ImGui::DragInt("Port", &port, 1, 0, 60000))
			{
				mPort = static_cast<unsigned short>(port);
			}
			ImGui::InputText("Server Address", mServerAddress.data(), mServerAddress.capacity());
			if (ImGui::Button("Connect"))
			{
				StartNetwork(mServer, mServerAddress);
			}
		}
	}
}

void NetworkManager::ReceiveMsg()
{
	mNetwork->ReceiveMsg();
}
void NetworkManager::SendMsg(const char* msg, int length)
{
	mNetwork->SendMsg(msg, length);
}