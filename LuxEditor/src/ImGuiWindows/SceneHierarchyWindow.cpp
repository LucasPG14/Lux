#include "luxpch.h"
#include "SceneHierarchyWindow.h"

#include <ImGui/imgui.h>

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

		if (selected)
		{
			selected->DrawInspector();
		}

		ImGui::End();
		// Inspector end
	}
}