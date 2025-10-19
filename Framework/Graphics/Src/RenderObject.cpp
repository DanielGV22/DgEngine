#include "Precompiled.h"
#include "RenderObject.h"

using namespace DgEngine;
using namespace DgEngine::Graphics;

void RenderObject::Terminate()
{
	meshBuffer.Terminate();
}