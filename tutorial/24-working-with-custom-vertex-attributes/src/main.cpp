/*
Copyright (c) 2016 Aerys

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "minko/Minko.hpp"
#include "minko/MinkoSDL.hpp"

#include "MyCustomMaterial.hpp"

using namespace minko;
using namespace minko::math;
using namespace minko::component;

const math::uint WINDOW_WIDTH = 800;
const math::uint WINDOW_HEIGHT = 600;

geometry::Geometry::Ptr
createGeometryWithAttribute(render::AbstractContext::Ptr);

void
addCustomAttribute(render::AbstractContext::Ptr context, geometry::Geometry::Ptr geometry);

int
main(int argc, char** argv)
{
	auto canvas = Canvas::create("Minko Tutorial - Working with custom vertex attributes", WINDOW_WIDTH, WINDOW_HEIGHT);
	auto sceneManager = component::SceneManager::create(canvas);

	sceneManager->assets()->loader()->queue("effect/MyCustomEffect.effect");

	auto root = scene::Node::create("root")
		->addComponent(sceneManager);

	auto camera = scene::Node::create()
		->addComponent(Renderer::create(0x7f7f7fff))
		->addComponent(Transform::create(inverse(lookAt(vec3(0.f, 1.f, -5.f), vec3(), vec3(0.f, 1.f, 0.f)))))
		->addComponent(PerspectiveCamera::create((float)WINDOW_WIDTH / (float)WINDOW_HEIGHT));
	root->addChild(camera);

	auto cube = scene::Node::create("cube");

	auto complete = sceneManager->assets()->loader()->complete()->connect([&](file::Loader::Ptr loader)
	{
		auto myCustomMaterial = material::MyCustomMaterial::create();
		cube
			->addComponent(Transform::create())
			->addComponent(Surface::create(
				createGeometryWithAttribute(canvas->context()), // geometry with add. vertex attribute
				myCustomMaterial,
				sceneManager->assets()->effect("effect/MyCustomEffect.effect")
			));
		root->addChild(cube);
	});

	auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, float t, float dt)
	{
		auto transform = cube->component<Transform>();
		transform->matrix(transform->matrix() * rotate(.01f, vec3(0.f, 1.f, 0.f)));

		sceneManager->nextFrame(t, dt);
	});

	sceneManager->assets()->loader()->load();

	canvas->run();

	return 0;
}

geometry::Geometry::Ptr
createGeometryWithAttribute(render::AbstractContext::Ptr context)
{
	auto cubeGeometry = geometry::CubeGeometry::create(context); // original cube geometry

	auto numVertices = cubeGeometry->numVertices(); // 36 vertices (6 vertices per face)
	auto offsetData = std::vector<float>(3 * numVertices, 0.0f); // vec3 per vertex

	math::uint i = 0;
	for (math::uint vertexId = 0; vertexId < numVertices; ++vertexId)
	{
		float dx = 0.0f; // components of the normal displacement
		float dy = 0.0f;
		float dz = 0.0f;

		switch (vertexId / 6) // faceId
		{
		case 0:
			dy += 0.1f;
			break;
		case 1:
			dy -= 0.1f;
			break;
		case 2:
			dz -= 0.1f;
			break;
		case 3:
			dz += 0.1f;
			break;
		case 4:
			dx -= 0.1f;
			break;
		case 5:
			dx += 0.1f;
			break;
		}

		offsetData[i++] = dx;
		offsetData[i++] = dy;
		offsetData[i++] = dz;
	}

	auto offsetBuffer = render::VertexBuffer::create(context, offsetData);
	offsetBuffer->addAttribute("positionOffset", 3, 0);

	cubeGeometry->addVertexBuffer(offsetBuffer);

	addCustomAttribute(context, cubeGeometry);

	return cubeGeometry;
}

void
addCustomAttribute(render::AbstractContext::Ptr context, geometry::Geometry::Ptr geometry)
{
	const math::uint	numVertices = 36;
	auto		colorData = std::vector<float>(4 * numVertices, 0.0f); // vec4 per vertex

	math::uint i = 0;
	for (math::uint vId = 0; vId < numVertices; ++vId)
	{
		float r = 0.0f;
		float g = 0.0f;
		float b = 0.0f;
		float a = 1.0f;

		if (vId == 7 || vId == 10 || vId == 12 || vId == 16 || vId == 30 || vId == 35)
		{
			r = 1.0f;
		}
		else if (vId == 4 || vId == 13 || vId == 15 || vId == 25 || vId == 27)
		{
			g = 1.0f;
		}
		else if (vId == 6 || vId == 11 || vId == 19 || vId == 21 || vId == 24 || vId == 28)
		{
			b = 1.0f;
		}
		else if (vId == 0 || vId == 3 || vId == 14 || vId == 31)
		{
			r = 1.0f; g = 1.0f;
		}
		else if (vId == 8 || vId == 22 || vId == 34)
		{
			r = 1.0f; b = 1.0f;
		}
		else if (vId == 1 || vId == 5 || vId == 18 || vId == 29)
		{
			g = 1.0f; b = 1.0f;
		}
		else if (vId == 2 || vId == 20 || vId == 23 || vId == 32 || vId == 33)
		{
			r = 1.0f; g = 1.0f; b = 1.0f;
		}
		colorData[i++] = r;
		colorData[i++] = g;
		colorData[i++] = b;
		colorData[i++] = a;
	}

	auto colorBuffer = render::VertexBuffer::create(context, colorData);
	colorBuffer->addAttribute("color", 4, 0);

	geometry->addVertexBuffer(colorBuffer);
}
