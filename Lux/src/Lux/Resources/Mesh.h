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
		Mesh(const AABB& aabb, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& ind, const std::string& p, const std::string& origPath);
		virtual ~Mesh();

		void SetID(int meshID = -1) { id = meshID; }
		int GetID() { return id; }

		const std::string& GetPath() { return path; }
		const std::string& GetOriginalPath() { return originalPath; }
		UUID GetUUID() { return uuid; }

		const AABB& GetAABB() { return aabb; }
		
		const std::vector<glm::vec4>& GetPositions() { return positions; }
		const std::vector<glm::vec4>& GetIndicesRT() { return indicesRT; }
		const std::vector<glm::vec4>& GetNormals() { return normals; }

	private:
		UUID uuid;
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

		std::vector<glm::vec4> positions;
		std::vector<glm::vec4> normals;
		std::vector<glm::vec4> indicesRT;

		std::string path;
		std::string originalPath;

		AABB aabb;

		int id;
	};
}