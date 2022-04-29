#include "amtpch.h"
#include "ContentBrowserWindow.h"

#include "Amethyst/Resources/ResourceSystem.h"
#include "Amethyst/Core/KeyCodes.h"

#include "Amethyst/Utils/Importer.h"

#include <ImGui/imgui.h>

namespace Amethyst
{
	#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

	extern const std::filesystem::path assetsDir = "assets/";
	
	ContentBrowserWindow::ContentBrowserWindow() : currentDir(assetsDir)
	{
		//folder = Texture2D::Create("editor/textures/folder.png");
		//tex = Texture2D::Create("assets/textures/bakeHouse.png");

		//// Load Resources
		//std::stack<std::filesystem::path> resources;
		//resources.push(assetsDir);

		//std::string extension = ".bsres";

		//while (!resources.empty())
		//{
		//	std::filesystem::path path = resources.top();
		//	resources.pop();

		//	for (auto& entry : std::filesystem::directory_iterator(path))
		//	{
		//		const std::filesystem::path& filePath = entry.path();
		//		if (entry.is_directory()) resources.push(entry);
		//		else if (filePath.extension().string() == extension)
		//		{
		//			ResourceSystem::ImportResources(filePath);
		//		}
		//	}
		//}
	}
	
	void ContentBrowserWindow::Render()
	{
		// Content Browser begin
		static bool ret = true;
		ImGui::Begin("Content Browser", &ret, ImGuiWindowFlags_MenuBar);

		//ImGui::BeginMenuBar();

		//ImGui::EndMenuBar();

		//focused = false;
		//if (ImGui::IsWindowFocused()) focused = true;

		//// Setting the number of columns
		//float cell = 128;

		//float width = ImGui::GetContentRegionAvail().x;
		//int columns = (int)(width / cell);

		//if (columns < 1)
		//	columns = 1;

		//ImGui::Columns(columns, 0, false);

		//int i = 0;
		//for (const auto& file : std::filesystem::directory_iterator(currentDir))
		//{
		//	ImGui::PushID(i++);
		//	const auto& path = file.path();
		//	auto relativePath = std::filesystem::relative(file.path(), assetsDir);
		//	std::string filename = relativePath.filename().string();

		//	bool fileSelected = false;
		//	if (path == selected) fileSelected = true;

		//	if (fileSelected) ImGui::PushStyleColor(ImGuiCol_Button, { 0.992f, 0.894f, 0.764f, 0.4f });

		//	if (file.is_directory()) ImGui::ImageButton((ImTextureID)folder->GetID(), { 100, 100 }, { 0, 1 }, { 1, 0 });
		//	else ImGui::ImageButton((ImTextureID)tex->GetID(), { 100, 100 }, { 0, 1 }, { 1, 0 });
		//	
		//	if (fileSelected) ImGui::PopStyleColor();

		//	// Drag objects from the Content Browser
		//	if (ImGui::BeginDragDropSource())
		//	{
		//		const wchar_t* filePath = relativePath.c_str();
		//		ImGui::SetDragDropPayload("CONTENT_BROWSER", filePath, (wcslen(filePath) + 1) * sizeof(const wchar_t), ImGuiCond_Once);
		//		ImGui::EndDragDropSource();
		//	}

		//	if (ImGui::IsItemHovered())
		//	{
		//		if (ImGui::IsMouseDoubleClicked(0))
		//		{
		//			if (file.is_directory()) currentDir /= path.filename();
		//			//else if (path.extension().string() == ".bsscene") OpenScene(path);
		//		}
		//		else if (ImGui::IsMouseClicked(0))
		//		{
		//			selected = path;
		//		}
		//	}

		//	ImGui::TextWrapped(filename.c_str());

		//	ImGui::NextColumn();
		//	ImGui::PopID();
		//}

		//if (!ImGui::IsAnyItemHovered() && focused)
		//{
		//	if (ImGui::IsMouseClicked(0)) selected = "";
		//}

		ImGui::End();
		// Content Browser end
	}
	
	void ContentBrowserWindow::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);

		dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(ContentBrowserWindow::OnKeyPressed));
		dispatcher.Dispatch<WindowDropEvent>(BIND_EVENT_FN(ContentBrowserWindow::FileDropped));
	}
	
	bool ContentBrowserWindow::OnKeyPressed(KeyPressedEvent& e)
	{
		switch (e.GetKeyCode())
		{
		case Keys::DEL:
			if (focused)
			{
				std::ifstream file(selected, std::ios::binary);

				uint64_t uuid = 0;
				file.read((char*)&uuid, sizeof(uint64_t));

				ResourceSystem::Delete(uuid);
				std::filesystem::remove_all(selected);
			}
			break;
		}

		return true;
	}
	
	bool ContentBrowserWindow::FileDropped(WindowDropEvent& e)
	{
		std::vector<std::string>& paths = e.GetPaths();
		std::string extension = ".png";
		for (int i = 0; i < paths.size(); ++i)
		{
			std::filesystem::path path = paths[i];
			if (path.extension() == extension)
			{
				Importer::ImportTexture(path, currentDir);
				continue;
			}
			Importer::Import(path, currentDir);
		}

		return true;
	}
}