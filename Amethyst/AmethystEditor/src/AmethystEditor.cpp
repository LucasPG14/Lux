#include <Amethyst.h>

#include "imgui/imgui.h"

class ExampleLayer : public Amethyst::Layer
{
public:
	ExampleLayer() : Layer("Example") {}

	~ExampleLayer() {}

	void Update() override
	{
		AMT_INFO("ExampleLayer::Update");
	}

	void RenderImGui() override
	{
		ImGui::Begin("Hello");
		ImGui::End();
	}

	void OnEvent(Amethyst::Event& event) override
	{
		AMT_TRACE("{0}", event);
	}
};

class AmethystEditor : public Amethyst::Application
{
public:
	AmethystEditor()
	{
		PushLayer(new ExampleLayer);
	}

	~AmethystEditor()
	{

	}

};

Amethyst::Application* Amethyst::CreateApp()
{
	return new AmethystEditor();
}