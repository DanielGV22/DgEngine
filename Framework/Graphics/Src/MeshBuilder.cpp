#include"Precompiled.h"
#include"MeshBuilder.h"

using namespace DgEngine;
using namespace DgEngine::Graphics;

namespace
{
	Color GetNextColor(int& index)
	{
		constexpr Color colorTable[] = {
			Colors::AliceBlue,
			Colors::AntiqueWhite,
			Colors::Aqua,
			Colors::Aquamarine,
			Colors::Azure,
			Colors::Black,
			Colors::PaleVioletRed,
			Colors::MediumVioletRed,
			Colors::OrangeRed,
			Colors::HotPink,
			Colors::DeepPink,
			Colors::Crimson,
			Colors::LightSeaGreen,
		};

		index=(index+1)%std::size(colorTable);
		return colorTable[index];
	}
}

MeshPC MeshBuilder::CreateCubePC(float size, const Color& color)
{
	MeshPC mesh;
	const float hs = size * 0.5f;
	// front									 
	mesh.vertices.push_back({ { -hs, -hs, -hs }, color });
	mesh.vertices.push_back({ { -hs, hs, -hs },  color});
	mesh.vertices.push_back({ { hs, hs, -hs },   color});
	mesh.vertices.push_back({ { hs, -hs, -hs },  color });
	// back										 
	mesh.vertices.push_back({ { -hs, -hs, hs }, color});
	mesh.vertices.push_back({ { -hs, hs, hs },  color});
	mesh.vertices.push_back({ { hs, hs, hs },   color });
	mesh.vertices.push_back({ { hs, -hs, hs },  color});

	mesh.indices = {
		// front
		0,1,2,
		0,2,3,
		// back
		7,5,4,
		7,6,5,
		// right
		3,2,6,
		3,6,7,
		// left
		4,5,1,
		4,1,0,
		// top
		1,5,6,
		1,6,2,
		// bottom
		0,3,7,
		0,7,4
	};

	return mesh;

}

MeshPC MeshBuilder::CreateCubePC(float size)
{
	MeshPC mesh;

	int index = rand() % 100;
	const float hs = size * 0.5f;
	// front									 
	mesh.vertices.push_back({ { -hs, -hs, -hs },GetNextColor(index) });
	mesh.vertices.push_back({ { -hs, hs, -hs }, GetNextColor(index) });
	mesh.vertices.push_back({ { hs, hs, -hs },  GetNextColor(index) });
	mesh.vertices.push_back({ { hs, -hs, -hs }, GetNextColor(index) });
	// back										 
	mesh.vertices.push_back({ { -hs, -hs, hs }, GetNextColor(index) });
	mesh.vertices.push_back({ { -hs, hs, hs },  GetNextColor(index) });
	mesh.vertices.push_back({ { hs, hs, hs },   GetNextColor(index) });
	mesh.vertices.push_back({ { hs, -hs, hs },  GetNextColor(index) });
											
	mesh.indices = {
		// front
		0,1,2,
		0,2,3,
		// back
		7,5,4,
		7,6,5,
		// right
		3,2,6,
		3,6,7,
		// left
		4,5,1,
		4,1,0,
		// top
		1,5,6,
		1,6,2,
		// bottom
		0,3,7,
		0,7,4
	};

	return mesh;

}

MeshPC DgEngine::Graphics::MeshBuilder::CreatePyramidPC(float size)
{
	MeshPC mesh;

	int index = rand() % 100;
	const float hs = size * 0.5f;

	// vertices for pyramid

	 // Base vertices (bottom square)
	mesh.vertices.push_back({ { -hs, -hs, -hs }, GetNextColor(index) }); // 0: bottom left back
	mesh.vertices.push_back({ { -hs, -hs, hs }, GetNextColor(index) });  // 1: bottom left front
	mesh.vertices.push_back({ { hs, -hs, hs }, GetNextColor(index) });   // 2: bottom right front
	mesh.vertices.push_back({ { hs, -hs, -hs }, GetNextColor(index) });  // 3: bottom right back

	// Apex (top point)
	mesh.vertices.push_back({ { 0.0f, hs, 0.0f }, GetNextColor(index) }); // 4: top center

	// Indices for the pyramid
	mesh.indices = {
		// Base (bottom square)
		0, 1, 2,
		0, 2, 3,

		// Front face
		1, 4, 2,

		// Right face
		2, 4, 3,

		// Back face
		3, 4, 0,

		// Left face
		0, 4, 1
	};
	

	return mesh;
}

MeshPC DgEngine::Graphics::MeshBuilder::CreateRectanglePC(float width, float height, float depth)
{
	MeshPC mesh;

	int index = rand() % 100;
	const float hw= width * 0.5f;
	const float hh= height * 0.5f;
	const float hd= depth * 0.5f;

	// vertices for rectangle

	 // Front face
	mesh.vertices.push_back({ { -hw, -hh, -hd }, GetNextColor(index) }); // 0: bottom left front
	mesh.vertices.push_back({ { -hw, hh, -hd }, GetNextColor(index) });  // 1: top left front
	mesh.vertices.push_back({ { hw, hh, -hd }, GetNextColor(index) });   // 2: top right front
	mesh.vertices.push_back({ { hw, -hh, -hd }, GetNextColor(index) });  // 3: bottom right front

	// Back face
	mesh.vertices.push_back({ { -hw, -hh, hd }, GetNextColor(index) });  // 4: bottom left back
	mesh.vertices.push_back({ { -hw, hh, hd }, GetNextColor(index) });   // 5: top left back
	mesh.vertices.push_back({ { hw, hh, hd }, GetNextColor(index) });    // 6: top right back
	mesh.vertices.push_back({ { hw, -hh, hd }, GetNextColor(index) });   // 7: bottom right back

	// Indices for the rectangular prism (same as cube)
	mesh.indices = {
		// front
		0, 1, 2,
		0, 2, 3,
		// back
		7, 5, 4,
		7, 6, 5,
		// right
		3, 2, 6,
		3, 6, 7,
		// left
		4, 5, 1,
		4, 1, 0,
		// top
		1, 5, 6,
		1, 6, 2,
		// bottom
		0, 3, 7,
		0, 7, 4
	};


	return mesh;
}

