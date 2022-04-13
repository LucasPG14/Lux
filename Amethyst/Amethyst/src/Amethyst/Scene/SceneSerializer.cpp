#include "amtpch.h"
#include "SceneSerializer.h"

#include "Components/TransformComponent.h"
#include "Components/MeshComponent.h"
#include "Components/MaterialComponent.h"

#include <yaml-cpp/yaml.h>

namespace Amethyst
{
	YAML::Emitter& operator<<(YAML::Emitter& emitter, const glm::vec3& vec)
	{
		emitter << YAML::Flow;
		emitter << YAML::BeginSeq << vec.x << vec.y << vec.z << YAML::EndSeq;
		return emitter;
	}


	SceneSerializer::SceneSerializer(const std::shared_ptr<Scene>& sceneSer) : scene(sceneSer)
	{
	}

	void SceneSerializer::Serialize(const std::filesystem::path& path)
	{
		YAML::Emitter emitter;

		// Scene map begin
		emitter << YAML::BeginMap;
		emitter << YAML::Key << "Scene" << YAML::Value << "Untitled";
		
		// Entities seq begin
		emitter << YAML::Key << "Entities" << YAML::BeginSeq;

		for (int i = 0; i < scene->world.size(); ++i)
		{
			Entity& entity = scene->world[i];

			// Entity map begin
			emitter << YAML::BeginMap;

			emitter << YAML::Key << "Name" << YAML::Value << entity.GetName();

			if (TransformComponent* comp = entity.Get<TransformComponent>())
			{
				emitter << YAML::Key << "TransformComponent";
				// Transform map begin
				emitter << YAML::BeginMap;

				emitter << YAML::Key << "Position" << YAML::Value << comp->GetPosition();
				emitter << YAML::Key << "Rotation" << YAML::Value << comp->GetRotation();
				emitter << YAML::Key << "Scale" << YAML::Value << comp->GetScale();

				emitter << YAML::EndMap;
				// Transform map end
			}

			if (MeshComponent* comp = entity.Get<MeshComponent>())
			{
				emitter << YAML::Key << "MeshComponent";
				// Mesh map begin
				emitter << YAML::BeginMap;

				emitter << YAML::Key << "Path" << YAML::Value << comp->GetMesh()->GetPath();

				emitter << YAML::EndMap;
				// Mesh map end
			}

			if (MaterialComponent* comp = entity.Get<MaterialComponent>())
			{
				emitter << YAML::Key << "MaterialComponent";
				// Material map begin
				emitter << YAML::BeginMap;

				emitter << YAML::Key << "Path" << YAML::Value << comp->GetMaterial()->GetPath();

				emitter << YAML::EndMap;
				// Material map end
			}

			emitter << YAML::EndMap; // Entity map end
		}

		emitter << YAML::EndSeq; // Entities seq end

		emitter << YAML::EndMap; // Scene map end

		std::ofstream file(path);
		file << emitter.c_str();
	}
	
	bool SceneSerializer::Deserialize(const std::filesystem::path& path)
	{
		return false;
	}
}