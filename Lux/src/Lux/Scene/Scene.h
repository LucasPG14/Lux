#pragma once

#include "Entity.h"

#include "Lux/Renderer/Shader.h"

namespace Lux
{
	class TransformComponent;
	class LightComponent;
	struct AABB;

	struct ObjectInfo
	{
		glm::vec4 info;
	};

	class Scene
	{
	public:
		Scene();
		~Scene();

		void Update();

		void CollectInformation();

		inline std::vector<Entity>& GetWorld() { return world; }

		Entity& CreateEntity(const std::string& name = "Entity");
		Entity& CreateEntityWithUUID(UUID id, const std::string& name = "Entity");
	
		void AddLight(TransformComponent* transform, LightComponent* light) { lights.emplace_back(transform,light); }

		const std::vector<std::pair<TransformComponent*, LightComponent*>>& GetLights() { return lights; }

		const std::vector<glm::mat4>& GetTransforms() { return transforms; }
		const std::vector<char>& GetTextures() { return textures; }
		const std::vector<AABB>& GetAABBs() { return verticesAndCoords; }
		const std::vector<glm::vec4>& GetPositions() { return positions; }
		const std::vector<glm::vec4>& GetIndices() { return indices; }
		const std::vector<glm::vec4>& GetNormals() { return normals; }
		const std::vector<ObjectInfo>& GetObjectsInfo() { return objectsInfo; }

	private:
		void DrawCube(const glm::vec3& color, const glm::vec3& pos = glm::vec3(0.0f), const glm::vec3& scale = glm::vec3(1.0f), const glm::vec3& rot = glm::vec3(0.0f));

	private:
		std::vector<Entity> world;
		
		std::vector<glm::mat4> transforms;
		std::vector<char> textures;
		std::vector<AABB> verticesAndCoords;
		std::vector<glm::vec4> positions;
		std::vector<glm::vec4> indices;
		std::vector<glm::vec4> normals;
		std::vector<ObjectInfo> objectsInfo;

		std::vector<std::pair<TransformComponent*, LightComponent*>> lights;

		std::shared_ptr<Shader> shader;

		friend class SceneSerializer;
		friend class SceneRenderer;
	};
}