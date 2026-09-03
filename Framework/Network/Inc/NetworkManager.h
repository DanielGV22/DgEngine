#pragma once

#include "NetworkElement.h"

namespace DgEngine::Network
{
	class NetworkElement;
	class NetworkController;

	class NetworkManager final
	{
	public:
		static void StaticInitialize(HWND handle);
		static void StaticTerminate();
		static NetworkManager* Get();

		NetworkManager() = default;
		~NetworkManager() = default;

		void Initialize(HWND handle);
		void StartNetwork(bool server, const std::string& serverAddress);
		void Update(float deltaTime);
		void Terminate();
		void DebugUI();

		void ReceiveMsg();
		void SendMsg(const char* msg, int length);

		const std::string& GetLocalId() const;
		const std::vector<std::string>& GetPlayerIds() const;
		float GetLatencyAverage() const;

		void SetNetworkController(const std::string& id, NetworkController* networkController);
		void RemoveNetworkController(const std::string& id);

	private:
		static LRESULT CALLBACK NetworkManagerMessageHandler(HWND window, UINT msg, WPARAM wParam, LPARAM lParam);

		bool mStarted = false;
		bool mConnected = false;
		bool mServer = false;
		bool mObtainLatency = false;
		bool mServerDoneLatencyCheck = false;
		int mLatencyCount = 0;
		float mLatencyTime = 0.0f;
		float mLatencyAverage = 0.0f;
		float mClientServerDelta = 0.0f;
		float mClockDelta = 0.0f;
		float mSentLatencyTime = 0.0f;
		float mGameTime = 0.0f;
		float mNextUpdateTime = 0.0f;
		std::vector<float> mLatencyCalcTime;
		unsigned short mPort = DEFAULT_PORT;
		std::string mServerAddress;
		NetworkElement* mNetwork = nullptr;
		HWND mWindow = nullptr;
		std::string mWriteMessage;
		std::vector<std::string> mPlayerIds;
		std::unordered_map<std::string, float> mNextSetIdAttemp;
		std::unordered_map<std::string, NetworkController*> mNetworkControllers;
	};
}