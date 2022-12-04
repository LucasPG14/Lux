#include <Lux.h>
#include <Lux/Core/EntryPoint.h>

#include "RendererLayer.h"

namespace Lux
{
	class LuxOfflineRenderer : public Application
	{
	public:
		LuxOfflineRenderer()
		{
			PushLayer(new RendererLayer);
		}

		~LuxOfflineRenderer()
		{

		}

	};

	Application* CreateApp()
	{
		return new LuxOfflineRenderer();
	}
}