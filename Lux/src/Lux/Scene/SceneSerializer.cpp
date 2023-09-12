#include "luxpch.h"
#include "SceneSerializer.h"

#include "Components/TransformComponent.h"
#include "Components/MeshComponent.h"
#include "Components/MaterialComponent.h"
#include "Components/LightComponent.h"

#include "Lux/Resources/ResourceManager.h"

#include "Lux/Resources/Mesh.h"
#include "Lux/Resources/Material.h"

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

	template<>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};
}

namespace Lux
{
	YAML::Emitter& operator<<(YAML::Emitter& emitter, const glm::vec3& vec)
	{
		emitter << YAML::Flow;
		emitter << YAML::BeginSeq << vec.x << vec.y << vec.z << YAML::EndSeq;
		return emitter;
	}

	YAML::Emitter& operator<<(YAML::Emitter& emitter, const glm::vec4& vec)
	{
		emitter << YAML::Flow;
		emitter << YAML::BeginSeq << vec.x << vec.y << vec.z << vec.w << YAML::EndSeq;
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
			Entity* entity = scene->world[i];

			// Entity map begin
			emitter << YAML::BeginMap;

			emitter << YAML::Key << "UUID" << YAML::Value << entity->GetUUID();
			emitter << YAML::Key << "Name" << YAML::Value << entity->GetName();

			if (TransformComponent* comp = entity->Get<TransformComponent>())
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

			if (MeshComponent* comp = entity->Get<MeshComponent>())
			{
				emitter << YAML::Key << "MeshComponent";
				// Mesh map begin
				emitter << YAML::BeginMap;

				emitter << YAML::Key << "MeshPath" << YAML::Value << comp->GetMesh()->GetOriginalPath();

				emitter << YAML::EndMap;
				// Mesh map end
			}

			if (MaterialComponent* comp = entity->Get<MaterialComponent>())
			{
				emitter << YAML::Key << "MaterialComponent";
				// Material map begin
				emitter << YAML::BeginMap;

				emitter << YAML::Key << "Color" << YAML::Value << comp->GetMaterial()->GetColor();
				emitter << YAML::Key << "Metallic" << YAML::Value << comp->GetMaterial()->GetMetallic();
				emitter << YAML::Key << "Roughness" << YAML::Value << comp->GetMaterial()->GetRoughness();
				emitter << YAML::Key << "IndexRefraction" << YAML::Value << comp->GetMaterial()->GetRefractionIndex();
				emitter << YAML::Key << "Transmission" << YAML::Value << comp->GetMaterial()->GetTransmission();
				emitter << YAML::Key << "Emissive" << YAML::Value << comp->GetMaterial()->GetEmissive();
				emitter << YAML::Key << "Emission" << YAML::Value << comp->GetMaterial()->GetEmission();
				emitter << YAML::Key << "Absorption" << YAML::Value << comp->GetMaterial()->GetAbsorption();

				emitter << YAML::EndMap;
				// Material map end
			}

			if (LightComponent* comp = entity->Get<LightComponent>())
			{
				emitter << YAML::Key << "LightComponent";
				// Material map begin
				emitter << YAML::BeginMap;

				emitter << YAML::Key << "Light Type" << YAML::Value << int(comp->GetType());
				emitter << YAML::Key << "Color" << YAML::Value << comp->GetColor();
				emitter << YAML::Key << "Intensity" << YAML::Value << comp->GetIntensity();

				emitter << YAML::EndMap;
				// Material map end
			}

			emitter << YAML::EndMap; // Entity map end
		}

		emitter << YAML::EndSeq; // Entities seq end

		emitter << YAML::EndMap; // Scene map end

		std::ofstream file(path);
		file << emitter.c_str();
		file.close();
	}
	
	bool SceneSerializer::Deserialize(const std::filesystem::path& path)
	{
		scene->Reset();

		YAML::Node data = YAML::LoadFile(path.string());

		if (!data["Scene"])
		{
			LUX_CORE_WARN("Couldn't deserialize the scene {0}", path);
			return false;
		}

		YAML::Node entities = data["Entities"];

		for (YAML::iterator::value_type yamlEntity : entities)
		{
			const std::string& name = yamlEntity["Name"].as<std::string>();
			uint64_t uuid = yamlEntity["UUID"].as<uint64_t>();

			Entity* entity = scene->CreateEntityWithUUID(uuid, name);

			YAML::Node transform = yamlEntity["TransformComponent"];
			if (transform)
			{
				TransformComponent* comp = entity->Get<TransformComponent>();
				comp->SetPosition(transform["Position"].as<glm::vec3>());
				comp->SetRotation(transform["Rotation"].as<glm::vec3>());
				comp->SetScale(transform["Scale"].as<glm::vec3>());
			}

			YAML::Node mesh = yamlEntity["MeshComponent"];
			if (mesh)
			{
				std::string meshPath = mesh["MeshPath"].as<std::string>();
				entity->CreateComponent<MeshComponent>(meshPath);
				YAML::Node material = yamlEntity["MaterialComponent"];
				const std::shared_ptr<Material>& mat = entity->CreateComponent<MaterialComponent>()->GetMaterial();
				mat->SetColor(material["Color"].as<glm::vec3>());
				mat->SetMetallic(material["Metallic"].as<float>());
				mat->SetRoughness(material["Roughness"].as<float>());
				mat->SetRefractionIndex(material["IndexRefraction"].as<float>());
				mat->SetTransmission(material["Transmission"].as<float>());
				mat->SetEmissive(material["Emission"].as<bool>());
				mat->SetEmissiveColor(material["Emissive"].as<glm::vec3>());
				mat->SetAbsorption(material["Absorption"].as<float>());

				mat->SetDiffuse(nullptr);
				mat->SetNormalMap(nullptr);
				mat->SetMetallicMap(nullptr);
				mat->SetRoughnessMap(nullptr);
			}
			
			YAML::Node light = yamlEntity["LightComponent"];
			if (light)
			{
				LightType type = LightType(light["Light Type"].as<int>());
				LightComponent* comp = entity->CreateComponent<LightComponent>(type);
				comp->SetColor(light["Color"].as<glm::vec3>());
				comp->SetIntensity(light["Intensity"].as<float>());

				scene->AddLight(entity->Get<TransformComponent>(), comp);
			}
		}

		scene->SetPath(path.string());

		return true;
	}
}