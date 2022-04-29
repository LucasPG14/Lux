#include <Amethyst.h>
#include <Amethyst/Core/EntryPoint.h>

//#include "EditorLayer.h"

namespace Amethyst
{
	class AmethystEditor : public Application
	{
	public:
		AmethystEditor()
		{
			//PushLayer(new EditorLayer);
		}

		~AmethystEditor()
		{

		}

	};

	Application* CreateApp()
	{
		return new AmethystEditor();
	}
}