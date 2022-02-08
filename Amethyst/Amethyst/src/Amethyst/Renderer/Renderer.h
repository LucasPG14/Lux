#pragma once

namespace Amethyst
{
	enum class RendererAPI
	{
		NONE = 0,
		OPENGL = 1,
		VULKAN = 2
	};

	class Renderer
	{
	public:
		inline static RendererAPI GetRenderer() { return rendererType; }
	private:
		static RendererAPI rendererType;
	};
}