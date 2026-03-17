#pragma once

#include "DgEngine/Inc/DgEngine.h"

enum class CycleDirection
{
    Next,
    Previous
};

class CycleAnimationEvent : public DgEngine::Core::Event
{
public:
    CycleAnimationEvent(CycleDirection dir)
        : direction(dir)
    {
    }

    static size_t StaticGetTypeId()
    {
        static size_t id = std::hash<std::string>{}("CycleAnimationEvent");
        return id;
    }

    size_t GetTypeId() const override
    {
        return StaticGetTypeId();
    }

    CycleDirection direction;
};