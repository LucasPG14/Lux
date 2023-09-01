#include "luxpch.h"
#include "SceneHierarchyWindow.h"

#include <Lux.h>

#include "Lux/Resources/ResourceManager.h"

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

		std::vector<Entity*>& entities = scene->GetWorld();
		for (int i = 0; i < entities.size(); ++i)
		{
			Entity* entity = entities[i];
			ImGui::PushID(i);
			int flags = ImGuiTreeNodeFlags_Leaf;
			if (selected == entity) flags |= ImGuiTreeNodeFlags_Selected;
			if (ImGui::TreeNodeEx(entity->GetName().c_str(), flags))
			{
				ImGui::TreePop();
			}

			if (ImGui::IsItemClicked())
				selected = entity;

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
			if (material.GetDiffuse() != nullptr)
			{
				ImGui::ImageButton((ImTextureID)material.GetDiffuse()->GetID(), { 40.0f, 40.0f });
			}
			else
			{
				ImGui::Image(0, { 40.0f, 40.0f });
			}
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER"))
				{
					const wchar_t* path = (const wchar_t*)payload->Data;
					std::filesystem::path realPath = std::filesystem::path("Assets") / path;
					if (realPath.has_extension())
					{
						if (realPath.extension().string() == ".png" || realPath.extension().string() == ".jpg")
						{
							material.SetDiffuse(ResourceManager::GetTexture(realPath.string()));
							scene->Changed(Change::OBJECT);
						}
					}
				}

				ImGui::EndDragDropTarget();
			}

			ImGui::SameLine();
			ImGui::Text("Diffuse Map");

			if (material.GetNormalMap() != nullptr)
			{
				ImGui::ImageButton((ImTextureID)material.GetNormalMap()->GetID(), { 40.0f, 40.0f });
			}
			else
			{
				ImGui::Image(0, { 40.0f, 40.0f });
			}
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER"))
				{
					const wchar_t* path = (const wchar_t*)payload->Data;
					std::filesystem::path realPath = std::filesystem::path("Assets") / path;
					if (realPath.has_extension())
					{
						if (realPath.extension().string() == ".png" || realPath.extension().string() == ".jpg")
						{
							material.SetNormalMap(ResourceManager::GetTexture(realPath.string()));
							scene->Changed(Change::OBJECT);
						}
					}
				}

				ImGui::EndDragDropTarget();
			}
			ImGui::SameLine();
			ImGui::Text("Normal Map");

			if (material.GetMetallicMap() != nullptr)
			{
				ImGui::ImageButton((ImTextureID)material.GetMetallicMap()->GetID(), { 40.0f, 40.0f });
			}
			else
			{
				ImGui::Image(0, { 40.0f, 40.0f });
			}
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER"))
				{
					const wchar_t* path = (const wchar_t*)payload->Data;
					std::filesystem::path realPath = std::filesystem::path("Assets") / path;
					if (realPath.has_extension())
					{
						if (realPath.extension().string() == ".png" || realPath.extension().string() == ".jpg")
						{
							material.SetMetallicMap(ResourceManager::GetTexture(realPath.string()));
							scene->Changed(Change::OBJECT);
						}
					}
				}

				ImGui::EndDragDropTarget();
			}
			ImGui::SameLine();
			ImGui::Text("Metallic Map");

			if (material.GetRoughnessMap() != nullptr)
			{
				ImGui::ImageButton((ImTextureID)material.GetRoughnessMap()->GetID(), { 40.0f, 40.0f });
			}
			else
			{
				ImGui::Image(0, { 40.0f, 40.0f });
			}
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER"))
				{
					const wchar_t* path = (const wchar_t*)payload->Data;
					std::filesystem::path realPath = std::filesystem::path("Assets") / path;
					if (realPath.has_extension())
					{
						if (realPath.extension().string() == ".png" || realPath.extension().string() == ".jpg")
						{
							material.SetRoughnessMap(ResourceManager::GetTexture(realPath.string()));
							scene->Changed(Change::OBJECT);
						}
					}
				}

				ImGui::EndDragDropTarget();
			}
			ImGui::SameLine();
			ImGui::Text("Roughness Map");

			if (ImGui::ColorPicker4("##color", glm::value_ptr(material.GetColor())))
			{
				scene->Changed(Change::MATERIAL);
			}

			if (ImGui::DragFloat("Metallic", &material.GetMetallic(), 0.05f, 0.0f, 1.0f))
			{
				scene->Changed(Change::MATERIAL);
			}

			if (ImGui::DragFloat("Refraction Index", &material.GetRefractionIndex(), 0.05f, 0.0f))
			{
				scene->Changed(Change::MATERIAL);
			}

			if (ImGui::DragFloat("Roughness", &material.GetRoughness(), 0.05f, 0.0f, 1.0f))
			{
				scene->Changed(Change::MATERIAL);
			}

			if (ImGui::DragFloat("Transmission", &material.GetTransmission(), 0.05f, 0.0f, 1.0f))
			{
				scene->Changed(Change::MATERIAL);
			}

			if (ImGui::ColorPicker4("##emissive", glm::value_ptr(material.GetEmissive())))
			{
				scene->Changed(Change::MATERIAL);
			}
		}
		}

		if (LightComponent* component = entity.Get<LightComponent>())
		{
			if (ImGui::CollapsingHeader("Light Component"))
			{
				std::string type = component->GetType() == LightType::DIRECTIONAL ? "Directional" : "Point";
				if (ImGui::BeginCombo("Light Type", type.c_str()))
				{
					if (ImGui::Selectable("Directional")) component->SetType(LightType::DIRECTIONAL);
					if (ImGui::Selectable("Point")) component->SetType(LightType::POINT);
					scene->Changed(Change::LIGHT);
					ImGui::EndCombo();
				}
				if (ImGui::ColorPicker3("Light Color", glm::value_ptr(component->GetModifiedColor())))
				{
					scene->Changed(Change::LIGHT);
				}
				//ImGui::DragFloat("Cut Off", &(component->GetModifiedCutOff()), 1.0f, 0.0f, 180.0f);
				if (ImGui::DragFloat("Radius", &(component->GetModifiedRange()), 1.0f, 0.0f))
				{
					scene->Changed(Change::LIGHT);
				}
			}
		}

		if (ImGui::BeginCombo("##", "AddComponent"))
		{
			if (!entity.Get<MeshComponent>() && !entity.Get<LightComponent>())
			{
				ImGui::Selectable("Mesh Component");
			}
			if (entity.Get<LightComponent>() && !entity.Get<MeshComponent>())
			{
				ImGui::Selectable("Light Component");
				scene->Changed(Change::OBJECT);
			}
			ImGui::EndCombo();
		}
	}
}