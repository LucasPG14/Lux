#pragma once

#include "Lux/Core/Layer.h"

#include "Lux/Events/ApplicationEvent.h"
#include "Lux/Events/KeyEvent.h"
#include "Lux/Events/MouseEvent.h"

namespace Lux
{
	class LUX_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();

		~ImGuiLayer();

		void OnCreate() override;
		void OnDestroy() override;

		void Update(Timer timer) override;
		void RenderImGui() override;

		void Begin();
		void End();

	//	void OnEvent(Event& event) override;
	//private:
	//	bool OnMousePressed(MouseButtonPressedEvent& event);
	//	bool OnMouseReleased(MouseButtonReleasedEvent& event);
	//	bool OnMouseMoved(MouseMovedEvent& event);
	//	bool OnMouseScrolled(MouseScrolledEvent& event);

	//	bool OnKeyPressed(KeyPressedEvent& event);
	//	bool OnKeyReleased(KeyReleasedEvent& event);
	//	bool OnKeyTyped(KeyTypedEvent& event);
	//	bool OnWindowResized(WindowResizeEvent& event);
	};
}