#pragma once

#include "Lux/Core/Core.h"
#include "Lux/Events/Event.h"

#include "Lux/Core/Timer.h"

namespace Lux
{
	class LUX_API Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		virtual void OnCreate() {}
		virtual void OnDestroy() {}

		virtual void Update(Timer timer) {}
		virtual void RenderImGui() {}

		virtual void OnEvent(Event& e) {}

		inline const std::string& GetName() const { return name; }

	private:
		std::string name;
	};
}