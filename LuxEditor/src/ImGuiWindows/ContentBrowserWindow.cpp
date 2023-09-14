#include "luxpch.h"
#include "ContentBrowserWindow.h"

#include "Lux/Core/KeyCodes.h"

#include "Lux/Resources/ResourceManager.h"
#include "Lux/Utils/Importer.h"

#include <ImGui/imgui.h>

namespace Lux
{
	extern const std::filesystem::path assetsDir = "Assets";
	
	ContentBrowserWindow::ContentBrowserWindow() : currentDir(assetsDir)
	{
		folder = CreateSharedPtr<Texture2D>("Editor/Textures/folder.png");
		sceneIcon = CreateSharedPtr<Texture2D>("Editor/Textures/sceneIcon.png");
		modelsIcon = CreateSharedPtr<Texture2D>("Editor/Textures/modelsIcon.png");

		for (auto& entry : std::filesystem::directory_iterator("Library"))
		{
			ResourceManager::LoadFile(entry.path());
		}

		std::stack<std::string> stack;
		stack.push("Assets");
		while (!stack.empty())
		{
			std::string path = stack.top();
			stack.pop();
			for (auto& entry : std::filesystem::directory_iterator(path))
			{
				if (entry.is_directory())
				{
					stack.push(entry.path().string());
				}
				else
				{
					if (entry.path().extension() == ".png" || entry.path().extension() == ".jpg")
					{
						ResourceManager::CreateTexture(entry.path().string());
					}
				}
			}
		}
	}
	
	void ContentBrowserWindow::Render()
	{
		// Content Browser begin
		static bool ret = true;
		ImGui::Begin("Content Browser", &ret);

		if (currentDir.has_parent_path())
		{
			if (ImGui::Button("<-", { 20, 20 }))
			{
				currentDir = currentDir.parent_path();
				selected = "";
			}
		}

		focused = false;
		if (ImGui::IsWindowFocused()) focused = true;

		// Setting the number of columns
		float cell = 128;

		float width = ImGui::GetContentRegionMax().x;
		int columns = (int)(width / cell);

		if (columns < 1)
			columns = 1;

		ImGui::Columns(columns, 0, false);

		int i = 0;
		for (const auto& file : std::filesystem::directory_iterator(currentDir))
		{
			ImGui::PushID(i++);
			const auto& path = file.path();
			auto relativePath = std::filesystem::relative(file.path(), assetsDir);
			std::string filename = relativePath.filename().string();

			bool fileSelected = false;
			if (path == selected) fileSelected = true;

			if (fileSelected) ImGui::PushStyleColor(ImGuiCol_Button, { 0.992f, 0.894f, 0.764f, 0.4f });

			if (file.is_directory()) ImGui::ImageButton((ImTextureID)folder->GetID(), { 100, 100 }, { 0, 1 }, { 1, 0 });
			else if (file.path().extension() == ".png" || file.path().extension() == ".jpg")
			{
				if (ResourceManager::GetTexture(file.path().string()) == nullptr)
				{
					ResourceManager::CreateTexture(file.path().string());
				}
				ImGui::ImageButton((ImTextureID)ResourceManager::GetTexture(file.path().string())->GetID(), {100, 100}, {0, 1}, {1, 0});
			}
			else if (file.path().extension() == ".fbx" || file.path().extension() == ".obj")
			{
				ImGui::ImageButton((ImTextureID)modelsIcon->GetID(), { 100, 100 }, { 0, 1 }, { 1, 0 });
			}
			else if (file.path().extension() == ".scene")
			{
				ImGui::ImageButton((ImTextureID)sceneIcon->GetID(), { 100, 100 }, { 0, 1 }, { 1, 0 });
			}
			else
			{
				ImGui::ColorButton("##Image", {0.1f, 0.1f, 0.1f, 1.0f}, 0,{ 100, 100 });
			}
			
			if (fileSelected) ImGui::PopStyleColor();

			// Drag objects from the Content Browser
			if (ImGui::BeginDragDropSource())
			{
				const wchar_t* filePath = relativePath.c_str();
				ImGui::SetDragDropPayload("CONTENT_BROWSER", filePath, (wcslen(filePath) + 1) * sizeof(const wchar_t), ImGuiCond_Once);
				ImGui::EndDragDropSource();
			}

			if (ImGui::IsItemHovered())
			{
				if (ImGui::IsMouseDoubleClicked(0))
				{
					if (file.is_directory()) currentDir /= path.filename();
				}
				else if (ImGui::IsMouseClicked(0))
				{
					selected = path;
				}
			}

			ImGui::TextWrapped(filename.c_str());

			ImGui::NextColumn();
			ImGui::PopID();
		}

		if (!ImGui::IsAnyItemHovered() && focused)
		{
			if (ImGui::IsMouseClicked(0)) selected = "";
		}

		ImGui::End();
		// Content Browser end
	}
	
	void ContentBrowserWindow::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);

		dispatcher.Dispatch<KeyPressedEvent>(LUX_BIND_EVENT_FN(ContentBrowserWindow::OnKeyPressed));
		dispatcher.Dispatch<WindowDropEvent>(LUX_BIND_EVENT_FN(ContentBrowserWindow::FileDropped));
	}
	
	bool ContentBrowserWindow::OnKeyPressed(KeyPressedEvent& e)
	{
		if (!focused)
			return false;

		return true;
	}
	
	bool ContentBrowserWindow::FileDropped(WindowDropEvent& e)
	{
		std::vector<std::string>& paths = e.GetPaths();
		for (int i = 0; i < paths.size(); ++i)
		{
			std::filesystem::copy(paths[i], assetsDir);
			std::filesystem::path newPath = assetsDir / std::filesystem::path(paths[i]).filename();
			if (newPath.extension() == ".png" || newPath.extension() == ".jpg")
			{
				ResourceManager::CreateTexture(paths[i]);
			}
		}

		return true;
	}
}