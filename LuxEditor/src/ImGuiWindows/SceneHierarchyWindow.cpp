#include "luxpch.h"
#include "SceneHierarchyWindow.h"

#include <Lux.h>
#include <ImGui/imgui.h>
#include <glm/gtc/type_ptr.hpp>

namespace Lux
{
	static bool IsInsideWindow(ImVec2 size, ImVec2 windowPos, ImVec2 mousePos)
	{
		return mousePos.x > windowPos.x && mousePos.x < windowPos.x + size.x &&
			mousePos.y > windowPos.y && mousePos.y < windowPos.y + size.y;
	}

	SceneHierarchyWindow::SceneHierarchyWindow(const std::shared_ptr<Scene>& currScene) : scene(currScene), selected(nullptr)
	{

	}
	
	void SceneHierarchyWindow::Render()
	{
		// Hierarchy Begin
		ImGui::Begin("Hierarchy");

		focused = false;
		if (ImGui::IsWindowFocused()) focused = true;

		std::vector<Entity>& entities = scene->GetWorld();
		for (int i = 0; i < entities.size(); ++i)
		{
			Entity& entity = entities[i];
			ImGui::PushID(i);
			int flags = ImGuiTreeNodeFlags_Leaf;
			if (selected == &entity) flags |= ImGuiTreeNodeFlags_Selected;
			if (ImGui::TreeNodeEx(entity.GetName().c_str(), flags))
			{
				ImGui::TreePop();
			}

			if (ImGui::IsItemClicked())
				selected = &entity;

			ImGui::PopID();
		}

		if (focused && !ImGui::IsAnyItemHovered() && ImGui::IsMouseClicked(0) && IsInsideWindow(ImGui::GetWindowSize(), ImGui::GetWindowPos(), ImGui::GetIO().MousePos))
			selected = nullptr;

		ImGui::End();
		// Hierarchy End

		// Inspector begin
		ImGui::Begin("Inspector");

		if (selected) DrawEntityComponents(*selected);

		ImGui::End();
		// Inspector end
	}

	// TODO: Try to change the way components are stored to avoid using the "GetModified" function
	void SceneHierarchyWindow::DrawEntityComponents(Entity& entity)
	{
		ImGui::InputText("##name", entity.GetModifiedName().data(), 128);

		// Transform
		if (TransformComponent* component = entity.Get<TransformComponent>())
		{
			if (ImGui::CollapsingHeader("Transform Component"))
			{
				ImGui::Text("Position");
				ImGui::SameLine();
				if (ImGui::DragFloat3("##Position", glm::value_ptr(component->GetModifiedPosition())))
				{
					scene->Changed(Change::TRANSFORM);
				}

				ImGui::Text("Rotation");
				ImGui::SameLine();
				if (ImGui::DragFloat3("##Rotation", glm::value_ptr(component->GetModifiedRotation())))
				{
					scene->Changed(Change::TRANSFORM);
				}

				ImGui::Text("Scale");
				ImGui::SameLine();
				if (ImGui::DragFloat3("##Scale", glm::value_ptr(component->GetModifiedScale())))
				{
					scene->Changed(Change::TRANSFORM);
				}
			}
		}
		
		// Mesh
		if (MeshComponent* component = entity.Get<MeshComponent>())
		{
			if (ImGui::CollapsingHeader("Mesh Component"))
			{

			}
		}

		// Material
		if (MaterialComponent* component = entity.Get<MaterialComponent>())
		{
			if (ImGui::CollapsingHeader("Material Component"))
			{
				Material& material = *component->GetMaterial();
				if (ImGui::Button("##Diffuse Map", { 40.0f, 40.0f }))
				{

				}
				ImGui::SameLine();
				ImGui::Text("Diffuse Map");

				if (ImGui::Button("##Normal Map", { 40.0f, 40.0f }))
				{

				}
				ImGui::SameLine();
				ImGui::Text("Normal Map");

				if (ImGui::Button("##Metallic Map", { 40.0f, 40.0f }))
				{

				}
				ImGui::SameLine();
				ImGui::Text("Metallic Map");

				if (ImGui::Button("##Roughness Map", { 40.0f, 40.0f }))
				{

				}
				ImGui::SameLine();
				ImGui::Text("Roughness Map");

				if (ImGui::ColorPicker3("##color", glm::value_ptr(material.GetColor())))
				{
					scene->Changed(Change::MATERIAL);
				}

				if (ImGui::DragFloat("Metallic", &material.GetMetallic(), 0.1f, 0.0f, 1.0f))
				{
					scene->Changed(Change::MATERIAL);
				}
			}
		}

		if (LightComponent* component = entity.Get<LightComponent>())
		{
			if (ImGui::CollapsingHeader("Light Component"))
			{
				ImGui::ColorPicker3("Light Color", glm::value_ptr(component->GetModifiedColor()));
				ImGui::DragFloat("Cut Off", &(component->GetModifiedCutOff()), 1.0f, 0.0f, 180.0f);
				ImGui::DragFloat("Range", &(component->GetModifiedRange()));
			}
		}
	}
}