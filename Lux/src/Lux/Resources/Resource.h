#pragma once

#include <string>

namespace Lux
{
	class Resource
	{
	public:
		virtual ~Resource() {}

		virtual const std::string& GetPath() = 0;
		virtual const std::string& GetName() = 0;
	};
}