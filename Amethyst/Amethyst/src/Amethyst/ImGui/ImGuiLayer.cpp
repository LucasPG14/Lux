#include "amtpch.h"
#include "ImGuiLayer.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
//#include "backends/imgui_impl_opengl3.h"

#include "ImGuiStyle.h"

#include "Amethyst/Core/Application.h"
#include "Platform/Vulkan/VulkanContext.h"

// TEMPORARY
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
//#include <glad/glad.h>

#include <ImGuizmo.h>
#include <optick.h>

namespace Amethyst
{
	ImGuiLayer::ImGuiLayer() : Layer("ImGuiLayer")
	{
	}
	
	ImGuiLayer::~ImGuiLayer()
	{
	}

	void ImGuiLayer::OnCreate()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;      // Enable keyboard controls
		// io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;    // Enable gamepad controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;          // Enable docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;        // Enable Multi-Viewport
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

		// Setup Dear ImGui style
		//ImGui::StyleColorsDark();
		ImGui::SetAmethystStyle();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform Windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		Application& app = Application::Get();
		GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetWindow());

		VkDescriptorPoolSize poolSizes[] =
		{
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
		};

		// Copied from ImGui example
		VkDescriptorPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		poolInfo.maxSets = 1000 * IM_ARRAYSIZE(poolSizes);;
		poolInfo.poolSizeCount = std::size(poolSizes);
		poolInfo.pPoolSizes = poolSizes;

		VkPhysicalDevice physDevice = VulkanContext::GetVkDevice().GetPhysicalDevice();

		vkCreateDescriptorPool(VulkanContext::GetVkDevice().GetDevice(), &poolInfo, nullptr, &descPool);

		ImGui_ImplVulkan_InitInfo initInfo = {};
		initInfo.Instance = VulkanContext::GetInstance();
		initInfo.PhysicalDevice = physDevice;
		initInfo.Device = VulkanContext::GetVkDevice().GetDevice();
		initInfo.Queue = VulkanContext::GetQueue();
		initInfo.DescriptorPool = descPool;
		initInfo.MinImageCount = 3;
		initInfo.ImageCount = 3;
		initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

		// Setup Platform/Rendering bindings
		ImGui_ImplGlfw_InitForVulkan(window, true);
		ImGui_ImplVulkan_Init(&initInfo, VulkanContext::GetRenderPass());
		//ImGui_ImplGlfw_InitForOpenGL(window, true);
		//ImGui_ImplOpenGL3_Init("#version 410");

		VkCommandBufferBeginInfo cmdBeginInfo = {};
		cmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		cmdBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		VkCommandBuffer command = VulkanContext::GetSpecCommandBuffer();
		vkBeginCommandBuffer(command, &cmdBeginInfo);
		
		ImGui_ImplVulkan_CreateFontsTexture(command);

		vkEndCommandBuffer(command);

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pCommandBuffers = &command;

		submitInfo.waitSemaphoreCount = 0;
		submitInfo.pWaitSemaphores = nullptr;
		submitInfo.pWaitDstStageMask = nullptr;
		submitInfo.commandBufferCount = 1;
		submitInfo.signalSemaphoreCount = 0;
		submitInfo.pSignalSemaphores = nullptr;

		vkResetFences(VulkanContext::GetVkDevice().GetDevice(), 1, &VulkanContext::GetFence()[0]);

		VkResult error = vkQueueSubmit(VulkanContext::GetQueue(), 1, &submitInfo, VulkanContext::GetFence()[0]);
		vkQueueWaitIdle(VulkanContext::GetQueue());

		vkResetCommandPool(VulkanContext::GetVkDevice().GetDevice(), VulkanContext::GetCommandPool(), 0);

		ImGui_ImplVulkan_DestroyFontUploadObjects();
	}

	void ImGuiLayer::OnDestroy()
	{
		//ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		vkDestroyDescriptorPool(VulkanContext::GetVkDevice().GetDevice(), descPool, VulkanContext::GetCallbackAllocator());
	}
	
	void ImGuiLayer::Update(Timer timer)
	{
	}

	void ImGuiLayer::RenderImGui()
	{
	}

	void ImGuiLayer::Begin()
	{
		OPTICK_EVENT("ImGui Layer Begin");

		//ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
	}

	void ImGuiLayer::End(uint32_t index)
	{
		OPTICK_EVENT("ImGui Layer End");

		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::Get();
		io.DisplaySize = ImVec2((float)app.GetWindow().GetWidth(), (float)app.GetWindow().GetHeight());

		VkRect2D scissors = {};
		scissors.offset = { 0,0 };
		scissors.extent = VulkanContext::GetExtent();
		vkCmdSetScissor(VulkanContext::GetSpecCommandBuffer(), 0, 1, &scissors);

		// Rendering
		ImGui::Render();
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), VulkanContext::GetSpecCommandBuffer());
		//ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			//GLFWwindow* window = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			//glfwMakeContextCurrent(window);
		}
	}
	
	/*void ImGuiOpenGLLayer::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);

		dispatcher.Dispatch<MouseButtonPressedEvent>(AMT_BIND_EVENT_FN(ImGuiLayer::OnMousePressed));
		dispatcher.Dispatch<MouseButtonReleasedEvent>(AMT_BIND_EVENT_FN(ImGuiLayer::OnMouseReleased));
		dispatcher.Dispatch<MouseMovedEvent>(AMT_BIND_EVENT_FN(ImGuiLayer::OnMouseMoved));
		dispatcher.Dispatch<MouseScrolledEvent>(AMT_BIND_EVENT_FN(ImGuiLayer::OnMouseScrolled));
		dispatcher.Dispatch<KeyPressedEvent>(AMT_BIND_EVENT_FN(ImGuiLayer::OnKeyPressed));
		dispatcher.Dispatch<KeyTypedEvent>(AMT_BIND_EVENT_FN(ImGuiLayer::OnKeyTyped));
		dispatcher.Dispatch<WindowResizeEvent>(AMT_BIND_EVENT_FN(ImGuiLayer::OnWindowResized));
	}
	
	bool ImGuiOpenGLLayer::OnMousePressed(MouseButtonPressedEvent& event)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseDown[event.GetMouseButton()] = true;

		return false;
	}
	
	bool ImGuiOpenGLLayer::OnMouseReleased(MouseButtonReleasedEvent& event)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseDown[event.GetMouseButton()] = false;

		return false;
	}
	
	bool ImGuiOpenGLLayer::OnMouseMoved(MouseMovedEvent& event)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MousePos = ImVec2(event.GetX(), event.GetY());

		return false;
	}
	
	bool ImGuiOpenGLLayer::OnMouseScrolled(MouseScrolledEvent& event)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseWheelH += event.GetXOffset();
		io.MouseWheel += event.GetYOffset();

		return false;
	}
	
	bool ImGuiOpenGLLayer::OnKeyPressed(KeyPressedEvent& event)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.KeysDown[event.GetKeyCode()] = true;

		io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
		io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
		io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
		io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];

		return false;
	}

	bool ImGuiOpenGLLayer::OnKeyTyped(KeyTypedEvent& event)
	{
		ImGuiIO& io = ImGui::GetIO();
		int key = event.GetKeyCode();

		if (key > 0 && key < 0x10000)
			io.AddInputCharacter((unsigned short)key);

		return false;
	}
	
	bool ImGuiOpenGLLayer::OnKeyReleased(KeyReleasedEvent& event)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.KeysDown[event.GetKeyCode()] = false;

		return false;
	}
	
	bool ImGuiOpenGLLayer::OnWindowResized(WindowResizeEvent& event)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2(event.GetWidth(), event.GetHeight());
		io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
		glViewport(0, 0, event.GetWidth(), event.GetHeight());

		return false;
	}*/
}