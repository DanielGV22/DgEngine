#pragma once

#include "Color.h"

namespace DgEngine::Graphics
{
  struct DirectionalLight
  {
	  Color ambient = Colors::White;                        // any light color without light contact
	  Color diffuse = Colors::White;                        // base light color
	  Color specular = Colors::White;                       // highlight light color
	  Math::Vector3 direction = Math::Vector3::ZAxis;       // light direction
	  float padding = 0.0f;                                 // padding to maintain struct 16 byte aligned
  };
}
