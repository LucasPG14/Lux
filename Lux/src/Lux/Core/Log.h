#pragma once

#include "Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace Lux
{
	class LUX_API Log
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
#define LUX_CORE_TRACE(...)    ::Lux::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define LUX_CORE_INFO(...)     ::Lux::Log::GetCoreLogger()->info(__VA_ARGS__)
#define LUX_CORE_WARN(...)     ::Lux::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define LUX_CORE_ERROR(...)    ::Lux::Log::GetCoreLogger()->error(__VA_ARGS__)
#define LUX_CORE_FATAL(...)    ::Lux::Log::GetCoreLogger()->fatal(__VA_ARGS__)

// Client log macros
#define LUX_TRACE(...)	      ::Lux::Log::GetClientLogger()->trace(__VA_ARGS__)
#define LUX_INFO(...)	      ::Lux::Log::GetClientLogger()->info(__VA_ARGS__)
#define LUX_WARN(...)	      ::Lux::Log::GetClientLogger()->warn(__VA_ARGS__)
#define LUX_ERROR(...)	      ::Lux::Log::GetClientLogger()->error(__VA_ARGS__)
#define LUX_FATAL(...)	      ::Lux::Log::GetClientLogger()->fatal(__VA_ARGS__)