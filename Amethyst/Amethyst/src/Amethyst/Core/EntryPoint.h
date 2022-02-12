#pragma once

#ifdef AMT_PLATFORM_WINDOWS

extern Amethyst::Application* Amethyst::CreateApp();

int main(int argc, char** argv)
{
	Amethyst::Log::Init();
	AMT_CORE_WARN("Initialized Log!");
	AMT_INFO("Hello!");

	auto app = Amethyst::CreateApp();
	app->Run();
	delete app;
}

#endif