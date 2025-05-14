#pragma once

#include"MeshTypes.h"

namespace DgEngine::Graphics
{

	class MeshBuilder 
	{
	public:
		//cube
		static MeshPC CreateCubePC(float size, const Color& color);
		static MeshPC CreateCubePC(float size);

		// pyramid
		static MeshPC CreatePyramidPC(float size);

		// rectangle
		static MeshPC CreateRectanglePC(float width, float height, float depth);
	};
}