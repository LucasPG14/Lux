#pragma once

#include "Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace Amethyst
{
	class AMT_API Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return coreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return clientLogger; }
	private:
		static std::shared_ptr<spdlog::logger> coreLogger;
		static std::shared_ptr<spdlog::logger> clientLogger;
	};
}

// Core log macros
#define AMT_CORE_TRACE(...)    ::Amethyst::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define AMT_CORE_INFO(...)     ::Amethyst::Log::GetCoreLogger()->info(__VA_ARGS__)
#define AMT_CORE_WARN(...)     ::Amethyst::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define AMT_CORE_ERROR(...)    ::Amethyst::Log::GetCoreLogger()->error(__VA_ARGS__)
#define AMT_CORE_FATAL(...)    ::Amethyst::Log::GetCoreLogger()->fatal(__VA_ARGS__)

// Client log macros
#define AMT_TRACE(...)	      ::Amethyst::Log::GetClientLogger()->trace(__VA_ARGS__)
#define AMT_INFO(...)	      ::Amethyst::Log::GetClientLogger()->info(__VA_ARGS__)
#define AMT_WARN(...)	      ::Amethyst::Log::GetClientLogger()->warn(__VA_ARGS__)
#define AMT_ERROR(...)	      ::Amethyst::Log::GetClientLogger()->error(__VA_ARGS__)
#define AMT_FATAL(...)	      ::Amethyst::Log::GetClientLogger()->fatal(__VA_ARGS__)