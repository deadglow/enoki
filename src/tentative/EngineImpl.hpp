#pragma once

#include "renderer/Renderer.hpp"
#include "renderer/ModelManager.hpp"
#include "renderer/ShaderManager.hpp"
#include "renderer/MaterialManager.hpp"
#include "Time.hpp"
#include "Input.hpp"
#include "physics/Physics.hpp"
#include "renderer/DebugRender.hpp"

namespace Enoki
{
	class Engine::Impl
	{
	public:
		Impl(const Graphics::Renderer& renderer, const Time& time)
			: renderer(renderer)
			, time(time)
		{
		}

		Time time;
		Input input;

		Graphics::ShaderManager shaderManager;
		Graphics::ModelManager modelManager;
		Graphics::MaterialManager materialManager;
		Graphics::Renderer renderer;
		Graphics::DebugRender debugRender;

		Physics::PhysicsSystem physicsSystem;

	};
}