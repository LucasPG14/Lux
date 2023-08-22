#include "luxpch.h"
#include "FileDialogs.h"

#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <commdlg.h>

#include "Lux/Core/Application.h"

namespace Lux
{
	std::string FileDialog::OpenFile(const char* filter)
	{
		OPENFILENAMEA openFileName;
		CHAR sizeFile[260] = { 0 };
		CHAR currDir[260] = { 0 };

		ZeroMemory(&openFileName, sizeof(OPENFILENAMEA));
		openFileName.lStructSize = sizeof(OPENFILENAMEA);
		openFileName.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Get().GetWindow().GetWindow());
		openFileName.lpstrFile = sizeFile;
		openFileName.nMaxFile = sizeof(sizeFile);
		openFileName.lpstrFilter = filter;
		openFileName.nFilterIndex = 1;
		openFileName.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

		if (GetOpenFileNameA(&openFileName) == TRUE)
			return openFileName.lpstrFile;

		return std::string();
	}
	
	std::string FileDialog::SaveFile(const char* filter)
	{
		OPENFILENAMEA openFileName;
		CHAR sizeFile[260] = { 0 };
		CHAR currDir[260] = { 0 };

		ZeroMemory(&openFileName, sizeof(OPENFILENAMEA));
		openFileName.lStructSize = sizeof(OPENFILENAMEA);
		openFileName.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Get().GetWindow().GetWindow());
		openFileName.lpstrFile = sizeFile;
		openFileName.nMaxFile = sizeof(sizeFile);
		openFileName.lpstrFilter = filter;
		openFileName.nFilterIndex = 1;
		openFileName.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

		if (GetSaveFileNameA(&openFileName) == TRUE)
			return openFileName.lpstrFile + openFileName.nFileExtension;

		return std::string();
	}
}