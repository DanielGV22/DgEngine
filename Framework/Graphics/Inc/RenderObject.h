#pragma once

#include "MeshBuffer.h"
#include "Transform.h"
#include "Material.h"
#include "TextureManager.h"

namespace DgEngine::Graphics
{
	class RenderObject
	{
	public:
		void Terminate();

		Transform  transform;                      // location/orientation
		MeshBuffer meshBuffer;                     // shape
		Material   material;                       // light data
		TextureId diffuseMapId;                    // diffuse texture for an object 
		TextureId specMapId;                       // speculartexture for an object
		TextureId normalMapId;                     // normal map texture for an object
		TextureId bumpMapId;                       // bump map texture for an object
	};
}