#pragma once

#include "Resource.h"
#include "Lux/Utils/Vertex.h"
#include "Lux/Core/UUID.h"

#include <glm/glm.hpp>

namespace Lux
{
	class VertexArray;
	class VertexBuffer;
	class IndexBuffer;

	class Mesh
	{
	public:
		Mesh();
		Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& ind, const std::string& p, const std::string& origPath);
		virtual ~Mesh();

		const std::string& GetPath() { return path; }
		const std::string& GetOriginalPath() { return originalPath; }
		UUID GetUUID() { return uuid; }
		//virtual const std::string& GetName() { return path; }

	private:
		UUID uuid;
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

		std::vector<glm::vec4> positions;
		std::vector<glm::vec4> normals;
		std::vector<glm::vec4> indicesRT;

		std::shared_ptr<VertexArray> vao;
		std::shared_ptr<VertexBuffer> vbo;
		std::shared_ptr<IndexBuffer> ebo;

		std::string path;
		std::string originalPath;
	};
}