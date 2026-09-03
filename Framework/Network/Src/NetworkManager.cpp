#include "Precompiled.h"
#include "NetworkManager.h"

#include "Client.h"
#include "Server.h"
#include "NetworkEvents.h"
#include "NetworkController.h"

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
	if (sNetworkManager != nullptr)
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

		char hostname[256];
		gethostname(hostname, sizeof(hostname));

		struct hostent* host = gethostbyname(hostname);
		ASSERT(host != nullptr, "NetworkManager: failed to get hostname~");

		struct in_addr addr;
		memcpy(&addr, host->h_addr_list[0], sizeof(struct in_addr));

		// give local computer IPV4 address
		mServerAddress = std::string(inet_ntoa(addr));

		mPlayerIds.push_back("Server");
	}
	else
	{
		mNetwork = new Client(mPort);
		mNetwork->Initialize(mWindow, serverAddress);

		mPlayerIds.push_back("Client");

		char msgData[100];
		// message format: eventType sender data
		sprintf_s(msgData, "%d %s", (int)EventType::Connected, GetLocalId().c_str());
		SendMsg(msgData, 100);
	}

	mConnected = false;
	mStarted = true;
	mObtainLatency = false;
	mServerDoneLatencyCheck = false;
	mLatencyCount = 0;
	mLatencyTime = 0.0f;
	mLatencyAverage = 0.0f;
	mClientServerDelta = 0.0f;
	mClockDelta = 0.0f;
	mSentLatencyTime = 0.0f;
	mGameTime = Core::TimeUtil::GetTime();

	sWindowsMessageHandler.Hook(mWindow, NetworkManagerMessageHandler);
}
void NetworkManager::Update(float deltaTime)
{
	// if not started, return
	if (!mStarted)
	{
		return;
	}

	// elapsed time from a monitored message (used for obtaining latency)
	float elapsedTime = Core::TimeUtil::GetTime() - mGameTime;












	// do not send update values if obtaining latency
	// this will result in a slower latency than actual, 
	// so systems may over compensate
	if (!mObtainLatency)
	{

		for (auto& nextSetData : mNextSetIdAttemp)
		{
			nextSetData.second -= deltaTime;
			if (nextSetData.second <= 0.0f)
			{
				nextSetData.second = 0.5f;
				// send SetId again since we dont know if player got already
				char msgData[100];
				// message format: eventType sender data
				sprintf_s(msgData, "%d %s %s", (int)EventType::SetId, GetLocalId().c_str(), nextSetData.first.c_str());
				SendMsg(msgData, 100);
			}
		}

		if (mPlayerIds.size() > 1)
		{
			auto itr = mNetworkControllers.find(mPlayerIds[0]);
			if (itr != mNetworkControllers.end())
			{
				EventInput input;
				if (itr->second->TryGetDirtyInput(input))
				{
					//char msgData[100];
					//sprintf_s(msgData, "%d %s %d %d %d %d", (int)EventType::Input, mPlayerIds[0].c_str(), input.moveX, input.moveY, input.jump, input.shiftSpeed);
					//SendMsg(msgData, 100);
				}
				mNextUpdateTime -= deltaTime;
				if (mNextUpdateTime < 0.0f)
				{
					const Graphics::Transform* localTransform = itr->second->GetLocalTransform();
					const Physics::RigidBody* rigidBody = itr->second->GetRigidBody();
					const Math::Vector3& pos = localTransform->position;
					const Math::Quaternion& rot = localTransform->rotation;
					const Math::Vector3 vel = rigidBody->GetVelocity();
					const Math::Vector3 angVel = rigidBody->GetAngularVelocity();
					float currentTime = Core::TimeUtil::GetTime();
					char msgData[256];
					sprintf_s(msgData, "%d %s %f %f %f %f %f %f %f %f %f %f %f %f %f %f",
						(int)EventType::Update, mPlayerIds[0].c_str(),
						pos.x, pos.y, pos.z, rot.x, rot.y, rot.z, rot.w,
						vel.x, vel.y, vel.z, angVel.x, angVel.y, angVel.z,
						currentTime);
					SendMsg(msgData, 256);

					mNextUpdateTime += POSITION_UPDATE_RATE;
				}
			}
		}
	}
	// client side syncing
	if (!mServer)
	{
		if (mObtainLatency && elapsedTime > mSentLatencyTime)
		{
			char msgData[100];
			mSentLatencyTime = elapsedTime;
			sprintf_s(msgData, "%d %s %f", (int)EventType::Ping, mPlayerIds[0].c_str(), mSentLatencyTime);
			SendMsg(msgData, 100);
		}
		// server has not received the "latency check done" message
		else if (!mObtainLatency && !mServerDoneLatencyCheck && mLatencyAverage > 0.0f)
		{
			char msgData[100];
			sprintf_s(msgData, "%d %s %.6f", (int)EventType::LatencyDone, mPlayerIds[0].c_str(), mLatencyAverage);
			SendMsg(msgData, 100);
		}
	}

	// if no data, return
	if (mNetwork->GetDataLength() <= 0)
	{
		return;
	}

	const char* data = mNetwork->GetData();
	LOG("[Network] %s", data);
	mConnected = true;
	const char* spacePtr = std::strchr(data, ' ');
	std::string dataStr = data;
	if (spacePtr != nullptr)
	{
		dataStr = std::string(data, spacePtr - data);
	}
	EventType event = (EventType)atoi(dataStr.c_str());
	switch (event)
	{
		// only done by server (client was connected)
	case EventType::Connected:
	{
		char msgData[100];
		std::string clientId = "Client" + std::to_string(mPlayerIds.size());
		// message format: eventType sender data
		sprintf_s(msgData, "%d %s %s", (int)EventType::SetId, GetLocalId().c_str(), clientId.c_str());
		SendMsg(msgData, 100);
		mNextSetIdAttemp[clientId] = 0.5f;
	}
	break;
	case EventType::SetId:
	{
		// consume data
		int eventType = 0;
		char senderId[100];
		char localId[100];
		sscanf_s(data, "%d %s %s", &eventType, senderId, (unsigned int)sizeof(senderId), localId, (unsigned int)sizeof(localId));
		mPlayerIds[0] = localId;
		if (std::find(mPlayerIds.begin(), mPlayerIds.end(), senderId) == mPlayerIds.end())
		{
			mPlayerIds.push_back(senderId);
		}

		char msgData[100];
		sprintf_s(msgData, "%d %s", (int)EventType::IdRecieved, GetLocalId().c_str());
		SendMsg(msgData, 100);
	}
	break;
	// only done by server
	case EventType::IdRecieved:
	{
		int eventType = 0;
		char senderId[100];
		sscanf_s(data, "%d %s", &eventType, senderId, sizeof(senderId));
		auto itr = mNextSetIdAttemp.find(senderId);
		if (itr != mNextSetIdAttemp.end())
		{
			mNextSetIdAttemp.erase(itr);
		}
		if (std::find(mPlayerIds.begin(), mPlayerIds.end(), senderId) == mPlayerIds.end())
		{
			mPlayerIds.push_back(senderId);
		}
	}
	break;
	case EventType::Input:
	{
		int eventType = 0;
		EventInput input;
		char senderId[100];
		sscanf_s(data, "%d %s %d %d %d %d", &eventType, senderId, (unsigned int)sizeof(senderId), &input.moveX, &input.moveY, &input.jump, &input.shiftSpeed);

		auto itr = mNetworkControllers.find(senderId);
		if (itr != mNetworkControllers.end())
		{
			itr->second->SetInput(input);
		}
	}
	break;
	case EventType::Sync:
	{
		int eventType = 0;

		char senderId[100];
		Math::Vector3 pos;
		Math::Quaternion rot;
		sscanf_s(data, "%d %s %f %f %f %f %f %f %f", &eventType, senderId, (unsigned int)sizeof(senderId),
			&pos.x, &pos.y, &pos.z, &rot.x, &rot.y, &rot.z, &rot.w);
		auto itr = mNetworkControllers.find(senderId);
		if (itr != mNetworkControllers.end())
		{
			itr->second->SetPosition(pos);
		}
		if (mServer)
		{
			itr = mNetworkControllers.find(GetLocalId());
			const Graphics::Transform* localTransform = itr->second->GetLocalTransform();
			const Math::Vector3& pos = localTransform->position;
			const Math::Quaternion& rot = localTransform->rotation;
			char msgData[100];
			sprintf_s(msgData, "%d %s %.4f %.4f %.4f %.4f %.4f %.4f, %.4f",
				(int)EventType::Sync, GetLocalId().c_str(), pos.x, pos.y, pos.z, rot.x, rot.y, rot.z, rot.w);
			SendMsg(msgData, 100);
			mGameTime = Core::TimeUtil::GetTime();
			mObtainLatency = true;
		}
		else
		{
			char msgData[100];
			mLatencyCount = 0;
			mSentLatencyTime = elapsedTime;
			sprintf_s(msgData, "%d %s %f", (int)EventType::Ping, GetLocalId().c_str(), mSentLatencyTime);
			SendMsg(msgData, 100);
			mGameTime = Core::TimeUtil::GetTime();
			mObtainLatency = true;
		}
	}
	break;
	case EventType::Ping:
	{
		int eventType = 0;
		char senderId[100];
		float fClientTime = 0.0f;
		sscanf_s(data, "%d %s %f", &eventType, senderId, (unsigned int)sizeof(senderId), &fClientTime);

		// return the server time
		char msgData[100];
		sprintf_s(msgData, "%d %s %f %f", (int)EventType::Pong, GetLocalId().c_str(), fClientTime, elapsedTime);
		SendMsg(msgData, 100);
	}
	break;
	case EventType::Pong:
	{
		if (!mObtainLatency)
		{
			break;
		}
		int eventType = 0;
		char senderId[100];
		float sentTime = 0.0f;
		float serverTime = 0.0f;
		sscanf_s(data, "%d %s %f %f", &eventType, senderId, (unsigned int)sizeof(senderId), &sentTime, &serverTime);

		float currElapsedTime = Core::TimeUtil::GetTime() - mGameTime;
		mLatencyTime = (elapsedTime - sentTime) / 2.0f;
		mClientServerDelta = currElapsedTime - serverTime;
		mClockDelta = mLatencyTime + mClientServerDelta;
		if (mLatencyCount < LATENCY_CHECK)
		{
			// sort latency when obtained
			if (mLatencyCalcTime.empty())
			{
				mLatencyCalcTime.push_back(mLatencyTime);
			}
			else
			{
				std::vector<float>::iterator itr;
				for (itr = mLatencyCalcTime.begin(); itr != mLatencyCalcTime.end(); ++itr)
				{
					if ((*itr) > mLatencyTime)
					{
						break;
					}
				}
				if (itr != mLatencyCalcTime.end())
				{
					mLatencyCalcTime.insert(itr, mLatencyTime);
				}
				else
				{
					mLatencyCalcTime.push_back(mLatencyTime);
				}
			}

			char msgData[100];
			mSentLatencyTime = currElapsedTime;
			sprintf_s(msgData, "%d %s %f", (int)EventType::Ping, GetLocalId().c_str(), mSentLatencyTime);
			SendMsg(msgData, 100);
		}
		else
		{
			// calculate the median latency
			float median = 0.0f;
			// if it is odd, the center index is the median
			if (mLatencyCalcTime.size() % 2 == 1)
			{
				int midIndex = (int)floor((float)mLatencyCalcTime.size() / 2.0f);
				median = mLatencyCalcTime[midIndex];
			}
			// else get the average of the 2 in the middle
			else
			{
				int midLow = mLatencyCalcTime.size() / 2;
				int midHigh = midLow + 1;
				median = (mLatencyCalcTime[midLow] + mLatencyCalcTime[midHigh]) / 2.0f;
			}
			// remove all values out of range
			float sum = 0.0f;
			int index = 0;
			for (int i = 0; i < mLatencyCalcTime.size(); ++i)
			{
				if (mLatencyCalcTime[index] > median + LATENCY_DEV ||
					mLatencyCalcTime[index] < median - LATENCY_DEV)
				{
					std::vector<float>::iterator itr = mLatencyCalcTime.begin() + index;
					mLatencyCalcTime.erase(itr);
				}
				else
				{
					sum += mLatencyCalcTime[index];
					++index;
				}
			}
			// calculate the latency average
			mLatencyAverage = sum / (float)mLatencyCalcTime.size();
			// send latency done msg
			char msgData[100];
			sprintf_s(msgData, "%d %s %.6f", (int)EventType::LatencyDone, GetLocalId().c_str(), mLatencyAverage);
			SendMsg(msgData, 100);
			mObtainLatency = false;
		}
		++mLatencyCount;
	}
	break;
	case EventType::LatencyDone:
	{
		float latencyAverage = 0.0f;
		int eventType = 0;
		char senderId[100];
		sscanf_s(data, "%d %s %f", &eventType, senderId, (unsigned int)sizeof(senderId), &latencyAverage);

		if (!mServer)
		{
			mServerDoneLatencyCheck = true;
		}
		else
		{
			mLatencyAverage = latencyAverage;
			char msgData[100];
			sprintf_s(msgData, "%d %s %.6f", (int)EventType::LatencyDone, GetLocalId().c_str(), mLatencyAverage);
			SendMsg(msgData, 100);
			mObtainLatency = false;
		}
	}
	break;
	case EventType::Update:
	{
		int eventType = 0;

		char senderId[100];
		Math::Vector3 pos;
		Math::Quaternion rot;
		Math::Vector3 vel;
		Math::Vector3 angvel;
		float currentTime = 0.0f;
		char msgData[256];
		sscanf_s(data, "%d %s %f %f %f %f %f %f %f %f %f %f %f %f %f %f",
			&eventType, senderId, (unsigned int)sizeof(senderId),
			&pos.x, &pos.y, &pos.z, &rot.x, &rot.y, &rot.z, &rot.w,
			&vel.x, &vel.y, &vel.z, &angvel.x, &angvel.y, &angvel.z,
			&currentTime);

		auto itr = mNetworkControllers.find(senderId);
		if (itr != mNetworkControllers.end())
		{
			Graphics::Transform targetTrans;
			// where is the other player now
			targetTrans.position = pos + vel * (2.0 * mLatencyAverage);
			// where should it be at the end
			targetTrans.position += vel * POSITION_UPDATE_RATE;
			// need to predict current location
			// startingPoint = pos + vel * mLatencyAverage
			// endPoint = startingPoint + vel * POSITION_UPDATE_RATE
			itr->second->SetTargetTransform(targetTrans, currentTime);
		}
	}
	break;
	default:
		ASSERT(false, "NetworkManager: Invalid network event");
		break;
	}

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
		if (mConnected || (mServer && mNetwork != nullptr))
		{
			ImGui::LabelText("LocalId", GetLocalId().c_str());
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
			if (!mObtainLatency)
			{
				ImGui::LabelText("LatencyAverage", "%.6f", mLatencyAverage);
			}

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

const std::string& NetworkManager::GetLocalId() const
{
	ASSERT(!mPlayerIds.empty(), "NetworkManager: no players are added");
	return mPlayerIds[0];
}
const std::vector<std::string>& NetworkManager::GetPlayerIds() const
{
	return mPlayerIds;
}

float NetworkManager::GetLatencyAverage() const
{
	return mLatencyAverage;
}

void NetworkManager::SetNetworkController(const std::string& id, NetworkController* networkController)
{
	mNetworkControllers[id] = networkController;
	// as long as it is not the server and the local id has info
	// start syncing position as well as latency to help with
	// better position estimates while moving
	if (!mServer && !mPlayerIds.empty() && id == GetLocalId())
	{
		const Graphics::Transform* localTransform = networkController->GetLocalTransform();
		const Math::Vector3& pos = localTransform->position;
		const Math::Quaternion& rot = localTransform->rotation;
		char msgData[100];
		sprintf_s(msgData, "%d %s %.4f %.4f %.4f %.4f %.4f %.4f, %.4f",
			(int)EventType::Sync, id.c_str(), pos.x, pos.y, pos.z, rot.x, rot.y, rot.z, rot.w);
		SendMsg(msgData, 100);
		mGameTime = Core::TimeUtil::GetTime();
		mObtainLatency = true;
	}
}
void NetworkManager::RemoveNetworkController(const std::string& id)
{
	auto itr = mNetworkControllers.find(id);
	if (itr != mNetworkControllers.end())
	{
		mNetworkControllers.erase(itr);
	}
}