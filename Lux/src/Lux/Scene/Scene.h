#pragma once

#include "Entity.h"

#include "Lux/Renderer/Shader.h"

namespace Lux
{
	class TransformComponent;
	class LightComponent;
	struct AABB;

	enum class Change
	{
		NONE = 0,
		TRANSFORM = 1,
		MATERIAL = 2,
		OBJECT = 3,
		LIGHT
	};

	struct ObjectInfo
	{
		glm::vec4 info;
	};

	struct MaterialInfo
	{
		glm::vec4 textureIDs;
		glm::vec4 color;
		glm::vec4 properties;
		glm::vec4 emissive;
	};

	class Scene
	{
	public:
		Scene();
		~Scene();

		void Update();

		void Reset();

		void CollectInformation();

		Change GetChange() { return change; }
		void Changed(Change ch) { change = ch; }

		inline std::vector<Entity*>& GetWorld() { return world; }

		const std::string& GetPath() { return path; }
		void SetPath(const std::string& p) { path = p; }

		Entity* CreateEntity(const std::string& name = "Entity");
		Entity* CreateEntityWithPath(const std::string& path, const std::string& name);
		Entity* CreateEntityWithUUID(UUID id, const std::string& name = "Entity");

		void DestroyEntity(Entity* ent);
	
		void AddLight(TransformComponent* transform, LightComponent* light) { lights.emplace_back(transform,light); }

		const std::vector<std::pair<TransformComponent*, LightComponent*>>& GetLights() { return lights; }

		const std::vector<glm::mat4>& GetTransforms() { return transforms; }
		const std::vector<glm::vec4>& GetMeshesInfo() { return meshesInfo; }
		const std::vector<MaterialInfo>& GetMaterialsInfo() { return materialsInfo; }
		const std::vector<char>& GetTextures() { return textures; }
		const std::vector<AABB>& GetAABBs() { return aabbs; }
		const std::vector<glm::vec4>& GetPositions() { return positions; }
		const std::vector<glm::vec4>& GetIndices() { return indices; }
		const std::vector<glm::vec4>& GetNormals() { return normals; }
		const std::vector<ObjectInfo>& GetObjectsInfo() { return objectsInfo; }

	private:
		void DrawCube(const std::string& path, const glm::vec4& color, const glm::vec3& pos = glm::vec3(0.0f), const glm::vec3& scale = glm::vec3(1.0f), const glm::vec3& rot = glm::vec3(0.0f));

	private:
		Change change;

		std::vector<Entity*> world;
		
		std::vector<glm::mat4> transforms;
		std::vector<char> textures;
		std::vector<AABB> aabbs;
		std::vector<glm::vec4> positions;
		std::vector<glm::vec4> indices;
		std::vector<glm::vec4> normals;
		std::vector<glm::vec4> meshesInfo;
		std::vector<ObjectInfo> objectsInfo;
		std::vector<MaterialInfo> materialsInfo;

		std::vector<std::pair<TransformComponent*, LightComponent*>> lights;

		std::shared_ptr<Shader> shader;

		std::string path;

		friend class SceneSerializer;
		friend class SceneRenderer;
	};
}