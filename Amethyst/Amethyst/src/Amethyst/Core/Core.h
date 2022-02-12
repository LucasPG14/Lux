#pragma once

#ifdef AMT_PLATFORM_WINDOWS
#if AMT_DYNAMIC_LINK
	#ifdef AMT_BUILD_DLL
		#define AMT_API __declspec(dllexport)
	#else
		#define AMT_API __declspec(dllimport)
	#endif // 
#else
	#define AMT_API
#endif
#endif

#ifdef AMT_DEBUG
	#define AMT_ENABLE_ASSERTS
#endif

#ifdef AMT_ENABLE_ASSERTS
	#define AMT_ASSERT(x, ...) { if(!(x)) { AMT_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define AMT_CORE_ASSERT(x, ...) { if(!(x)) { AMT_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define HZ_ASSERT(x, ...)
	#define HZ_CORE_ASSERT(x, ...)
#endif // AMT_ENAMBLE_ASSERTS

#define BIT(x) (1 << x)

#define AMT_BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)