#pragma once

#include "Component.h"

namespace DgEngine
{
	class UIComponent : public Component
	{
	public:
		virtual void Render() = 0;
	};
}