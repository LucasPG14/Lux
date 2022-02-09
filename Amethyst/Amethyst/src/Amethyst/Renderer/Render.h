#pragma once

#include <glm/glm.hpp>

#include "VertexArray.h" 

namespace Amethyst
{
	class Render
	{
	public:
		enum class API
		{
			NONE = 0,
			OPENGL = 1,
			VULKAN = 2
		};
	public:
		virtual void Init() const = 0;

		virtual void ClearColor(const glm::vec4& color) const = 0;
		virtual void Clear() const = 0;

		virtual void Draw(const std::shared_ptr<VertexArray>& vertexArray) = 0;
	
		inline static API GetAPI() { return api; }
	private:
		static API api;
	};
}