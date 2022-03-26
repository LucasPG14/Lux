#pragma once

#include <imgui.h>

namespace ImGui
{
	void SetAmethystStyle()
	{
		// Old
		// 0.094f, 0.0f, 0.15f, 1.0f
		// 0.05f, 0.0f, 0.08f, 1.0f

		// New
		// 0.109f, 0.109f, 0.109f, 1.0f
		// 0.188f, 0.164f, 0.094f, 1.0f
		// 0.992f, 0.894f, 0.764f, 1.0f
		// 0.964f, 0.964f, 0.964f, 1.0f

		ImGuiStyle& style = ImGui::GetStyle();

		ImVec4* colors = style.Colors;

		// Backgrounds
		colors[ImGuiCol_WindowBg] = ImVec4(0.109f, 0.109f, 0.109f, 1.0f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.109f, 0.109f, 0.109f, 1.0f);
		colors[ImGuiCol_Border] = ImVec4(0.109f, 0.109f, 0.109f, 1.0f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.188f, 0.164f, 0.094f, 0.4f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.188f, 0.164f, 0.094f, 0.4f);

		// Tabs
		colors[ImGuiCol_Tab] = ImVec4(0.188f, 0.164f, 0.094f, 1.0f);
		colors[ImGuiCol_TabActive] = ImVec4(0.992f, 0.894f, 0.764f, 0.85f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.964f, 0.964f, 0.964f, 0.6f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.188f, 0.164f, 0.094f, 1.0f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.188f, 0.164f, 0.094f, 1.0f);

		// Separator
		colors[ImGuiCol_Separator] = ImVec4(0.992f, 0.894f, 0.764f, 0.3f);

		// Buttons
		colors[ImGuiCol_Button] = ImVec4(0.992f, 0.894f, 0.764f, 0.4f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.992f, 0.894f, 0.764f, 0.7f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.992f, 0.894f, 0.764f, 1.0f);

		// Header
		colors[ImGuiCol_Header] = ImVec4(0.992f, 0.894f, 0.764f, 0.4f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.992f, 0.894f, 0.764f, 0.7f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.992f, 0.894f, 0.764f, 1.0f);
	}
}