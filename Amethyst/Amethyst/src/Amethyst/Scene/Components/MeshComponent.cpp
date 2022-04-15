#include "amtpch.h"
#include "MeshComponent.h"

#include <imgui.h>

namespace Amethyst
{
	MeshComponent::MeshComponent(std::shared_ptr<Mesh> meshPtr) : mesh(meshPtr)
	{
		mesh->Load();
	}
	
	MeshComponent::~MeshComponent()
	{
	}
	
	void MeshComponent::Update()
	{
	}
	
	void MeshComponent::DrawInspector()
	{
		if (ImGui::CollapsingHeader("Mesh"))
		{

		}
	}
}