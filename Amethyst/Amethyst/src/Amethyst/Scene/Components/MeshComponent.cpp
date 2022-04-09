#include "amtpch.h"
#include "MeshComponent.h"

// TODO: This needs to be deleted
#include "Amethyst/Renderer/Renderer.h"

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
		mesh->Update();
	}
	
	void MeshComponent::DrawInspector()
	{
		if (ImGui::CollapsingHeader("Mesh"))
		{

		}
	}
}