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

	SceneHierarchyWindow::SceneHierarchyWindow(const std::shared_ptr<Scene>& currScene) : focused(false), scene(currScene), selected(nullptr)
	{
		removeTex = CreateSharedPtr<Texture2D>("Editor/Textures/trashlogo.png");
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

		ImGui::PushID(&entity);
		// Transform
		if (TransformComponent* component = entity.Get<TransformComponent>())
		{
			if (ImGui::CollapsingHeader("Transform Component"))
			{
				ImGui::Text("Position");
				ImGui::SameLine();
				if (ImGui::DragFloat3("##Position", glm::value_ptr(component->GetModifiedPosition()), 0.1f))
				{
					scene->Changed(Change::TRANSFORM);
				}

				ImGui::Text("Rotation");
				ImGui::SameLine();
				if (ImGui::DragFloat3("##Rotation", glm::value_ptr(component->GetModifiedRotation()), 0.1f))
				{
					scene->Changed(Change::TRANSFORM);
				}

				ImGui::Text("Scale");
				ImGui::SameLine();
				if (ImGui::DragFloat3("##Scale", glm::value_ptr(component->GetModifiedScale()), 0.1f))
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
					ImGui::ImageButton((ImTextureID)material.GetDiffuse()->GetID(), { 20.0f, 20.0f }, { 0, 1 }, { 1, 0 });
				}
				else
				{
					ImGui::Image(0, { 20.0f, 20.0f });
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
				if (material.GetDiffuse() != nullptr)
				{
					ImGui::SameLine();
					if (ImGui::ImageButton((ImTextureID)removeTex->GetID(), { 20.0f, 20.0f }, { 0, 1 }, { 1, 0 }))
					{
						material.SetDiffuse(nullptr);
						scene->Changed(Change::OBJECT);
					}
				}

				ImGui::Text("Diffuse");
				ImGui::SameLine();
				if (ImGui::ColorEdit3("##Diffuse", glm::value_ptr(material.GetColor())))
				{
					scene->Changed(Change::MATERIAL);
				}
				ImGui::Separator();

				if (material.GetNormalMap() != nullptr)
				{
					ImGui::ImageButton((ImTextureID)material.GetNormalMap()->GetID(), { 20.0f, 20.0f }, { 0, 1 }, { 1, 0 });
				}
				else
				{
					ImGui::Image(0, { 20.0f, 20.0f });
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
				if (material.GetNormalMap() != nullptr)
				{
					ImGui::SameLine();
					if (ImGui::ImageButton((ImTextureID)removeTex->GetID(), { 20.0f, 20.0f }, { 0, 1 }, { 1, 0 }))
					{
						material.SetNormalMap(nullptr);
						scene->Changed(Change::OBJECT);
					}
				}

				ImGui::Separator();

				if (material.GetMetallicMap() != nullptr)
				{
					ImGui::ImageButton((ImTextureID)material.GetMetallicMap()->GetID(), { 20.0f, 20.0f }, { 0, 1 }, { 1, 0 });
				}
				else
				{
					ImGui::Image(0, { 20.0f, 20.0f });
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
				if (material.GetMetallicMap() != nullptr)
				{
					ImGui::SameLine();
					if (ImGui::ImageButton((ImTextureID)removeTex->GetID(), { 20.0f, 20.0f }, { 0, 1 }, { 1, 0 }))
					{
						material.SetMetallicMap(nullptr);
						scene->Changed(Change::OBJECT);
					}
				}

				ImGui::Text("Metallic");
				ImGui::SameLine();
				if (ImGui::DragFloat("##Metallic", &material.GetMetallic(), 0.01f, 0.0f, 1.0f))
				{
					scene->Changed(Change::MATERIAL);
				}
				ImGui::Separator();

				if (material.GetRoughnessMap() != nullptr)
				{
					ImGui::ImageButton((ImTextureID)material.GetRoughnessMap()->GetID(), { 20.0f, 20.0f }, { 0, 1 }, { 1, 0 });
				}
				else
				{
					ImGui::Image(0, { 20.0f, 20.0f });
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
				if (material.GetRoughnessMap() != nullptr)
				{
					ImGui::SameLine();
					if (ImGui::ImageButton((ImTextureID)removeTex->GetID(), { 20.0f, 20.0f }, { 0, 1 }, { 1, 0 }))
					{
						material.SetRoughnessMap(nullptr);
						scene->Changed(Change::OBJECT);
					}
				}

				ImGui::Text("Roughness");
				ImGui::SameLine();
				if (ImGui::DragFloat("##Roughness", &material.GetRoughness(), 0.01f, 0.0f, 1.0f))
				{
					scene->Changed(Change::MATERIAL);
				}

				ImGui::Separator();

				ImGui::Text("Refraction Index");
				ImGui::SameLine();
				if (ImGui::DragFloat("##Refraction Index", &material.GetRefractionIndex(), 0.01f, 1.1f))
				{
					scene->Changed(Change::MATERIAL);
				}

				ImGui::Text("Transmission");
				ImGui::SameLine();
				if (ImGui::DragFloat("##Transmission", &material.GetTransmission(), 0.01f, 0.0f, 1.0f))
				{
					scene->Changed(Change::MATERIAL);
				}

				ImGui::Text("Absorption");
				ImGui::SameLine();
				if (ImGui::DragFloat("##Absorption", &material.GetAbsorption(), 0.1f))
				{
					scene->Changed(Change::MATERIAL);
				}

				if (ImGui::ColorEdit3("##Emissive", glm::value_ptr(material.GetEmissive()), ImGuiColorEditFlags_NoInputs))
				{
					scene->Changed(Change::MATERIAL);
				}
				ImGui::SameLine();
				ImGui::Text("Emission");
				ImGui::SameLine();
				if (ImGui::Checkbox("##Emission", &material.GetEmission()))
				{
					scene->Changed(Change::MATERIAL);
				}
			}
		}

		if (LightComponent* component = entity.Get<LightComponent>())
		{
			if (ImGui::CollapsingHeader("Light Component"))
			{
				ImGui::Text("Light Color");
				ImGui::SameLine();
				if (ImGui::ColorEdit3("##Light Color", glm::value_ptr(component->GetModifiedColor()), ImGuiColorEditFlags_NoInputs))
				{
					scene->Changed(Change::LIGHT);
				}
				ImGui::Text("Intensity");
				ImGui::SameLine();
				if (ImGui::DragFloat("##Intensity", &(component->GetModifiedIntensity()), 0.1f, 0.1f, 100000.0f))
				{
					scene->Changed(Change::LIGHT);
				}
			}
		}

		//ImGui::Separator();

		//if (ImGui::BeginCombo("##", "AddComponent"))
		//{
		//	if (!entity.Get<MeshComponent>() && !entity.Get<LightComponent>())
		//	{
		//		ImGui::Selectable("Mesh Component");
		//	}
		//	if (entity.Get<LightComponent>() && !entity.Get<MeshComponent>())
		//	{
		//		ImGui::Selectable("Light Component");
		//		scene->Changed(Change::OBJECT);
		//	}
		//	ImGui::EndCombo();
		//}
		ImGui::PopID();
	}
}