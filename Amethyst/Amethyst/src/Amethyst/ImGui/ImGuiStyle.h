#pragma once

#include <imgui.h>

namespace ImGui
{
	void SetAmethystStyle()
	{
		// 0.094f, 0.0f, 0.15f, 1.0f
		// 0.05f, 0.0f, 0.08f, 1.0f

		ImGuiStyle& style = ImGui::GetStyle();

		ImVec4* colors = style.Colors;

		colors[ImGuiCol_WindowBg] = ImVec4(0.02f, 0.0f, 0.04f, 1.0f);
	}
}