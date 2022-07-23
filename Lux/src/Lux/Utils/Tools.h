#pragma once

#include "luxpch.h"

namespace Lux
{
	template<typename T, typename... Args>
	std::shared_ptr<T> CreateSharedPtr(Args&&... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	template<typename T, typename... Args>
	std::unique_ptr<T> CreateUniquePtr(Args&&... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}
}