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
		static MeshPX CreateCubePX(float size);

		// pyramid
		static MeshPC CreatePyramidPC(float size);

		// rectangle
		static MeshPC CreateRectanglePC(float width, float height, float depth);

		// plane 
		static MeshPC CreatePlanePC(int numRows,int numColums,float spacing, bool horizontal=true);
		static MeshPX CreatePlanePX(int numRows, int numColums, float spacing, bool horizontal = true);
		static Mesh CreatePlane(int numRows, int numColums, float spacing, bool horizontal = true);

		// cylinder
		static MeshPC CreateCylinderPC(int slices, int rings);

		// sphere
		static MeshPC CreateSpherePC(int slices, int rings, float radius);
		static MeshPX CreateSpherePX(int slices, int rings, float radius);
		static Mesh CreateSphere(int slices, int rings, float radius);

		// skyphere
		static MeshPX CreateSkySpherePX(int slices, int rings, float radius);

		// load obj
		static MeshPX CreateObjPX(const std::filesystem::path& filePath, float scale);

		// Screen quad
		static MeshPX CreateScreenQuadPX();
	};
}