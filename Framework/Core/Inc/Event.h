#pragma once

namespace DgEngine::Core
{
	using EventTypeId = std::size_t;

	class Event
	{
	public:
		Event() = default;
		virtual ~Event() = default;

		virtual EventTypeId GetTypeId() const = 0;

	};
}

#define SET_EVENT_TYPE_ID(id)\
	static DgEngine::Core::EventTypeId StaticGetTypeId() { return static_cast<DgEngine::Core::EventTypeId>(id); }\
	DgEngine::Core::EventTypeId GetTypeId() const override { return StaticGetTypeId(); }