#pragma once

#include "Resource.h"

#include "Amethyst/Renderer/Shader.h"
#include "Amethyst/Renderer/VertexArray.h"

namespace Amethyst
{
	class Mesh : public Resource
	{
	public:
		Mesh(UUID id, const std::filesystem::path& filePath);

		~Mesh();

		void Load() override;

		void UnLoad() override;

		const std::shared_ptr<VertexArray>& GetVAO() { return vao; }
		const std::shared_ptr<Shader>& GetShader() { return meshShader; }

		inline const std::vector<Vertex>& GetVertices() { return vertices; }
		inline const size_t GetIndicesSize() { return indices.size(); }

	private:
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

		std::shared_ptr<Shader> meshShader;
		std::shared_ptr<VertexArray> vao;
		AABB aabb;
	};
}