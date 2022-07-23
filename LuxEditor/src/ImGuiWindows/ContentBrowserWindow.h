#pragma once

#include "Lux/Resources/Texture2D.h"

#include "Lux/Events/Event.h"
#include "Lux/Events/ApplicationEvent.h"
#include "Lux/Events/KeyEvent.h"

namespace Lux
{
	class ContentBrowserWindow
	{
	public:
		ContentBrowserWindow();

		void Render();

		const std::filesystem::path& GetCurrentDir() { return currentDir; }

		void OnEvent(Event& e);

	private:
		bool OnKeyPressed(KeyPressedEvent& e);
		bool FileDropped(WindowDropEvent& e);

	private:
		bool focused;

		std::filesystem::path currentDir;
		std::filesystem::path selected;

		std::shared_ptr<Texture2D> tex;
		std::shared_ptr<Texture2D> folder;
	};
}