#pragma once

#include "Amethyst/Core/Core.h"
#include "Amethyst/Events/Event.h"

#include "Amethyst/Core/Timer.h"

namespace Amethyst
{
	class AMT_API Layer
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