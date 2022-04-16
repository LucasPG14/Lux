#pragma once

#include "Amethyst/Resources/Texture.h"

#include "Amethyst/Events/Event.h"
#include "Amethyst/Events/ApplicationEvent.h"
#include "Amethyst/Events/KeyEvent.h"

namespace Amethyst
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