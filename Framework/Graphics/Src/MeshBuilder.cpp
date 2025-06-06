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

void CreateCubeIndices(std::vector<uint32_t>& indices)
{
	indices = {
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
}

void CreatePlaneIndices(std::vector<uint32_t>& indices, int numRows, int numColums)
{
	indices.reserve(numRows * numColums * 6); // Preallocate space for indices

	for (int r = 0; r < numRows; ++r)
	{
		for (int c = 0; c < numColums; ++c)
		{
			int i = r * (numColums + 1) + c;

			// triangle 0
			indices.push_back(i);
			indices.push_back(i + numColums + 1);
			indices.push_back(i + numColums + 2);

			// First triangle
			indices.push_back(i);
			indices.push_back(i + numColums + 2);
			indices.push_back(i + 1);
		}
	}
}

void CreateCapIndices(std::vector<uint32_t>& indices, int slices, int topIndex, int bottomIndex)
{
	indices.reserve(slices * 6); // Preallocate space for indices

	for (int s = 0; s < slices; ++s)
	{
		// bottom triangle 
		indices.push_back(bottomIndex);
		indices.push_back(s);
		indices.push_back(s + 1);

		// top triangle
		int topRowIndex = topIndex - slices - 1 + s;
		indices.push_back(topIndex);
		indices.push_back(topRowIndex + 1);
		indices.push_back(topRowIndex);
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

	CreateCubeIndices(mesh.indices);

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
											
	CreateCubeIndices(mesh.indices);

	return mesh;

}

MeshPX MeshBuilder::CreateCubePX(float size)
{
	MeshPX mesh;



	return mesh;
}

MeshPC MeshBuilder::CreatePyramidPC(float size)
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

	
	CreateCubeIndices(mesh.indices);


	return mesh;
}

MeshPC MeshBuilder::CreatePlanePC(int numRows, int numColums, float spacing, bool horizontal)
{
	MeshPC mesh;
	int index = rand() % 100;
	const float hpw=static_cast<float>(numColums) * spacing * 0.5f;
	const float hph=static_cast<float>(numRows) * spacing * 0.5f;

	float w= -hpw;
	float h= -hph;

	for (int r = 0; r <= numRows; ++r)
	{
		for (int c = 0; c <= numColums; ++c)
		{
			Math::Vector3 pos= (horizontal)? Math::Vector3 {w, 0.0f, h } : Math::Vector3{ w, h, 0.0f };
			mesh.vertices.push_back({ pos, GetNextColor(index) });
			w += spacing;
		}
		w = -hpw;
		h += spacing;
	}

	CreatePlaneIndices(mesh.indices, numRows, numColums);

	return mesh; 
}

MeshPX MeshBuilder::CreatePlanePX(int numRows, int numColums, float spacing, bool horizontal)
{
	MeshPX mesh;

	const float hpw = static_cast<float>(numColums) * spacing * 0.5f;
	const float hph = static_cast<float>(numRows) * spacing * 0.5f;
	const float uInc = 1.0f / static_cast<float>(numColums);
	const float vInc = 1.0f / static_cast<float>(numRows);

	float w = -hpw;
	float h = -hph;
	float u = 0.0f;
	float v = 1.0f;

	for (int r = 0; r <= numRows; ++r)
	{
		for (int c = 0; c <= numColums; ++c)
		{
			Math::Vector3 pos = (horizontal) ? Math::Vector3{ w, 0.0f, h } : Math::Vector3{ w, h, 0.0f };
			mesh.vertices.push_back({ pos , { u , v} });
			w += spacing;
			u += uInc;
		}
		w = -hpw;
		h += spacing;
		u= 0.0f;
		v -= vInc;
	}

	CreatePlaneIndices(mesh.indices, numRows, numColums);

	return mesh;
}

MeshPC MeshBuilder::CreateCylinderPC(int slices, int rings)
{
	 MeshPC mesh;
	 int index= rand() % 100;

	 const float hh=static_cast<float>(rings) * 0.5f;
	 const float fSlices= static_cast<float>(slices);
	 for (int r = 0; r <= rings; ++r)
	 {
		 float ring = static_cast<float>(r);
		 for (int s = 0; s <= slices; ++s)
		 {
			 float slice = static_cast<float>(s);
			 float rotation = Math::Constants::TwoPi * (slice / fSlices);

			 mesh.vertices.push_back({ {
				 sin(rotation),
				 ring - hh,
				 -cos(rotation) },
		          GetNextColor(index) });
		 }
	 }

	 mesh.vertices.push_back({ { 0.0f, hh, 0.0f }, GetNextColor(index) }); // top center
	 mesh.vertices.push_back({ { 0.0f, -hh, 0.0f }, GetNextColor(index) }); // bottom center

	 CreatePlaneIndices(mesh.indices, rings, slices);
	 CreateCapIndices(mesh.indices, slices, mesh.vertices.size() - 2, mesh.vertices.size() - 1);

	 return mesh;
}

MeshPC MeshBuilder::CreateSpherePC(int slices, int rings, float radius)
{
	MeshPC mesh;
	int index = rand() % 100;

	float vertRotation = Math::Constants::Pi / static_cast<float>(rings);
	float horzRotation = Math::Constants::TwoPi / static_cast<float>(slices);

	for (int r = 0; r <= rings; ++r)
	{
		float ring = static_cast<float>(r);
		float phi = vertRotation * ring;
		for (int s = 0; s <= slices; ++s)
		{
			float slice = static_cast<float>(s);
			float rotation = horzRotation * slice;

			mesh.vertices.push_back({ {
				radius* sin(rotation) * sin(phi),
				radius* cos(phi),
				radius* cos(rotation) * sin(phi) },
			    GetNextColor(index) });
		}
	}

	CreatePlaneIndices(mesh.indices, rings, slices);

	return mesh;
}

MeshPX MeshBuilder::CreateSpherePX(int slices, int rings, float radius)
{
	 MeshPX mesh;



	 float vertRotation = Math::Constants::Pi / static_cast<float>(rings);
	 float horzRotation = Math::Constants::TwoPi / static_cast<float>(slices);

	 float uStep = 1.0f / static_cast<float>(slices);
	 float vStep = 1.0f / static_cast<float>(rings);

	 for (int r = 0; r <= rings; ++r)
	 {
		 float ring = static_cast<float>(r);
		 float phi = vertRotation * ring;
		 for (int s = 0; s <= slices; ++s)
		 {
			 float slice = static_cast<float>(s);
			 float rotation = horzRotation * slice;

			 float u=(uStep * slice);
			 float v=(vStep* ring);

			 mesh.vertices.push_back({ {
				 radius * sin(rotation) * sin(phi),
				 radius * cos(phi),
				 radius * cos(rotation) * sin(phi) },
				 {u,v} });
		 }
	 }

	 CreatePlaneIndices(mesh.indices, rings, slices);

	 return mesh;
}

MeshPX MeshBuilder::CreateSkySpherePX(int slices, int rings, float radius)
{
	MeshPX mesh;



	float vertRotation = Math::Constants::Pi / static_cast<float>(rings);
	float horzRotation = Math::Constants::TwoPi / static_cast<float>(slices);

	float uStep = 1.0f / static_cast<float>(slices);
	float vStep = 1.0f / static_cast<float>(rings);

	for (int r = 0; r <= rings; ++r)
	{
		float ring = static_cast<float>(r);
		float phi = vertRotation * ring;
		for (int s = 0; s <= slices; ++s)
		{
			float slice = static_cast<float>(s);
			float rotation = horzRotation * slice;

			float u = (uStep * slice);
			float v = (vStep * ring);

			mesh.vertices.push_back({ {
				radius * cos(rotation) * sin(phi),
				radius * cos(phi),
				radius * sin(rotation) * sin(phi) },
				{u,v} });
		}
	}

	CreatePlaneIndices(mesh.indices, rings, slices);

	return mesh;
}

MeshPX MeshBuilder::CreateObjPX(const std::filesystem::path& filePath, float scale)
{
	MeshPX mesh;
	FILE* file = nullptr;
	fopen_s(&file, filePath.u8string().c_str(), "r");
	ASSERT(file != nullptr, "MeshBuilder: can't open file %s", filePath.u8string().c_str());

	// read in file
	std::vector<Math::Vector3> positions;
	std::vector<Math::Vector2> uvCoords;
	std::vector<uint32_t> positionIndices;
	std::vector<uint32_t> uvIndices;

	while (true)
	{
		char buffer[128];
		int result = fscanf_s (file, "%s", buffer,(uint32_t)std::size(buffer));
		if (result == EOF)
		{
			break;
		}
		if (strcmp (buffer, "v") == 0)
		{
			float x, y, z = 0.0f;
			fscanf_s(file, "%f %f %f", &x, &y, &z);
			positions.push_back({ x, y, z });
		}
		else if (strcmp(buffer, "vt") == 0)
		{
			float u, v = 0.0f;
			fscanf_s(file, "%f %f", &u, &v);
			uvCoords.push_back({ u,1.0f - v });
		}
		else if (strcmp(buffer, "f") == 0)
		{
			uint32_t p[4];
			uint32_t uv[4];
			int count = fscanf_s(file, "%d/%d/%*d %d/%d/%*d %d/%d/%*d  %d/%d/%*d\n", &p[0], &uv[0], & p[1], &uv[1], & p[2], &uv[2],&p[3], &uv[3]);
			if (count % 3 == 0)
			{
				for (uint32_t i = 0; i < 3; i++)
				{
					positionIndices.push_back(p[i] - 1);
					uvIndices.push_back(uv[i] - 1);

				}
			}
			else
			{
				// triangle 1
				positionIndices.push_back(p[0] - 1);
				positionIndices.push_back(p[1] - 1);
				positionIndices.push_back(p[2] - 1);
				// triangle 2
				positionIndices.push_back(p[0] - 1);
				positionIndices.push_back(p[2] - 1);
				positionIndices.push_back(p[3] - 1);
				// triangle 1
				uvIndices.push_back(uv[0] - 1);
				uvIndices.push_back(uv[1] - 1);
				uvIndices.push_back(uv[2] - 1);
				//triangle 2
				uvIndices.push_back(uv[0] - 1);
				uvIndices.push_back(uv[2] - 1);
				uvIndices.push_back(uv[3] - 1);
			}
		}
	}
	fclose(file);
	mesh.vertices.resize(positions.size());
	for (uint32_t i = 0; i < positions.size(); ++i)
	{
		mesh.vertices[i].position = positions[i]* scale;
	}
	if (uvCoords.size() > 0)
	{
		for (uint32_t i = 0; i < uvIndices.size(); ++i)
		{
			mesh.vertices[positionIndices[i]].uvCoord = uvCoords[uvIndices[i]];
		}
	}
	mesh.indices = std::move(positionIndices);

	return mesh;
}

