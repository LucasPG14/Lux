#pragma once

#ifdef LUX_PLATFORM_WINDOWS

extern Lux::Application* Lux::CreateApp();

int main(int argc, char** argv)
{
	Lux::Log::Init();
	LUX_CORE_WARN("Initialized Log!");

	LUX_INFO("Initializing Lux Engine");
	auto app = Lux::CreateApp();
	app->Run();
	delete app;
}

#endif