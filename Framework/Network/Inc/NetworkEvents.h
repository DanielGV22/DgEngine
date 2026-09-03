#pragma once

namespace DgEngine::Network
{
	enum class EventType
	{
		Invalid = 0,
		Connected,
		SetId,
		IdRecieved,
		Input,
		Sync,
		Ping,
		Pong,
		LatencyDone,
		Update
	};

	struct EventSetPosition
	{
		EventSetPosition() = default;
		Math::Vector3 position;
	};

	struct EventInput
	{
		EventInput() = default;
		int moveX = 0;
		int moveY = 0;
		int jump = 0;
		int shiftSpeed = 0;
	};
}