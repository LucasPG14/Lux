#include "luxpch.h"
#include "MeshComponent.h"

#include "Lux/Utils/Primitives.h"
#include "Lux/Utils/Importer.h"
#include "Lux/Resources/ResourceManager.h"
#include "Lux/Resources/Mesh.h"

#include <imgui.h>

namespace Lux
{
	MeshComponent::MeshComponent(const std::string& path) : type(ComponentType::MESH), mesh(nullptr)
	{	
		if (ResourceManager::GetMesh(path) == nullptr)
		{
			mesh = Importer::ImportFBX2(path, "Assets/");
		}
		else
		{
			mesh = ResourceManager::GetMesh(path);
		}
	}
	
	MeshComponent::~MeshComponent()
	{
	}
	
	void MeshComponent::Update()
	{
	}
	
	const std::vector<glm::vec4>& MeshComponent::GetPositions()
	{
		return mesh->GetPositions();
	}
	
	const std::vector<glm::vec4>& MeshComponent::GetIndices()
	{
		return mesh->GetIndicesRT();
	}
	
	const std::vector<glm::vec4>& MeshComponent::GetNormals()
	{
		return mesh->GetNormals();
	}
}