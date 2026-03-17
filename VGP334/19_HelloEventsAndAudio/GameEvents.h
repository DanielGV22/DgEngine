#pragma once


#include <DgEngine/Inc/DgEngine.h>

enum class GameEventType
{
	PressSpace = 1,
	PressEnter
};

class PressSpaceEvent : public DgEngine::Core::Event
{
public:
	PressSpaceEvent() {}
	SET_EVENT_TYPE_ID(GameEventType::PressSpace)
};

class PressEnterEvent : public DgEngine::Core::Event
{
public:
	PressEnterEvent() {}
	SET_EVENT_TYPE_ID(GameEventType::PressEnter)
};