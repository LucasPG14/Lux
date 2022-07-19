#include "amtpch.h"
#include "SceneSerializer.h"

#include "Components/TransformComponent.h"
#include "Components/MeshComponent.h"
#include "Components/MaterialComponent.h"

#include <yaml-cpp/yaml.h>

// YAML functions to deserialize the glm::vec
namespace YAML
{
	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};
}

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

			emitter << YAML::Key << "UUID" << YAML::Value << entity.GetUUID();
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

				

				emitter << YAML::EndMap;
				// Mesh map end
			}

			if (MaterialComponent* comp = entity.Get<MaterialComponent>())
			{
				emitter << YAML::Key << "MaterialComponent";
				// Material map begin
				emitter << YAML::BeginMap;

				

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
		YAML::Node data = YAML::LoadFile(path.string());

		if (!data["Scene"])
		{
			AMT_CORE_WARN("Couldn't deserialize the scene {0}", path);
			return false;
		}

		YAML::Node entities = data["Entities"];

		for (YAML::iterator::value_type yamlEntity : entities)
		{
			const std::string& name = yamlEntity["Name"].as<std::string>();
			uint64_t uuid = yamlEntity["UUID"].as<uint64_t>();

			Entity& entity = scene->CreateEntityWithUUID(uuid, name);

			YAML::Node transform = yamlEntity["TransformComponent"];
			if (transform)
			{
				TransformComponent& comp = *entity.Get<TransformComponent>();
				comp.SetPosition(transform["Position"].as<glm::vec3>());
				comp.SetRotation(transform["Rotation"].as<glm::vec3>());
				comp.SetScale(transform["Scale"].as<glm::vec3>());
			}

			YAML::Node mesh = yamlEntity["MeshComponent"];
			if (mesh)
			{
				uint64_t uuid = mesh["UUID"].as<uint64_t>();
				
			}

			YAML::Node material = yamlEntity["MaterialComponent"];
			if (material)
			{
				uint64_t uuid = material["UUID"].as<uint64_t>();
				
			}
		}

		return true;
	}
}