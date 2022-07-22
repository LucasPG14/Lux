#pragma once

//#if _WIN32
//	#if _WIN64
//	#define LUX_PLATFORM_WINDOWS
//#endif // Windows

#if LUX_DYNAMIC_LINK
	#ifdef LUX_BUILD_DLL
		#define LUX_API __declspec(dllexport)
	#else
		#define LUX_API __declspec(dllimport)
	#endif // 
#else
	#define LUX_API

#endif
//#endif

#ifdef LUX_DEBUG
	#define LUX_ENABLE_ASSERTS
#endif

#ifdef LUX_ENABLE_ASSERTS
	#define LUX_ASSERT(x, ...) { if(!(x)) { LUX_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define LUX_CORE_ASSERT(x, ...) { if(!(x)) { LUX_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define LUX_ASSERT(x, ...)
	#define LUX_CORE_ASSERT(x, ...)
#endif // LUX_ENABLE_ASSERTS

#define BIT(x) (1 << x)

#define LUX_BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)