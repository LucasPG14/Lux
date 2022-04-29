#pragma once

#include "Amethyst/Core/Layer.h"

#include "Amethyst/Events/ApplicationEvent.h"
#include "Amethyst/Events/KeyEvent.h"
#include "Amethyst/Events/MouseEvent.h"

#include "backends/imgui_impl_vulkan.h"

namespace Amethyst
{
	class AMT_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();

		~ImGuiLayer();

		void OnCreate() override;
		void OnDestroy() override;

		void Update(Timer timer) override;
		void RenderImGui() override;

		void Begin();
		void End(uint32_t index);

	private:
		VkDescriptorPool descPool;
		VkCommandBuffer cmd;

		VkRenderPass renderPass;
		VkFramebuffer framebuffer;
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