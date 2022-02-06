#include <Amethyst.h>

class ExampleLayer : public Amethyst::Layer
{
public:
	ExampleLayer() : Layer("Example") {}

	~ExampleLayer() {}

	void Update() override
	{
		AMT_INFO("ExampleLayer::Update");
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
		PushOverlay(new Amethyst::ImGuiLayer);
	}

	~AmethystEditor()
	{

	}

};

Amethyst::Application* Amethyst::CreateApp()
{
	return new AmethystEditor();
}