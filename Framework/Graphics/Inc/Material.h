#pragma once
#include "Color.h"

namespace DgEngine::Graphics
{
	struct Material
	{
		Color emissive = Colors::Black;         // color where light is absent
		Color ambient = Colors::White;          // color where light is minimal
		Color diffuse = Colors::White;          // base light color
		Color specular = Colors::White;         // color for shiny areas
		float shininess = 10.0f;                // intensity of the light highlights
		float padding[3] = { 0.0f };            // padding to maintain 16 byte alignment
	};
}