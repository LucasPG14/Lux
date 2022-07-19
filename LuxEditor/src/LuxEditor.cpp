#include <Lux.h>
#include <Lux/Core/EntryPoint.h>

#include "EditorLayer.h"

namespace Lux
{
	class LuxEditor : public Application
	{
	public:
		LuxEditor()
		{
			PushLayer(new EditorLayer);
		}

		~LuxEditor()
		{

		}

	};

	Application* CreateApp()
	{
		return new LuxEditor();
	}
}