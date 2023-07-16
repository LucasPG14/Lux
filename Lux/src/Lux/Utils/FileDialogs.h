#pragma once

namespace Lux
{
	//// If the engine supports more than one system, this needs to change
	class FileDialog
	{
	public:
		static std::string OpenFile(const char* filter);

		static std::string SaveFile(const char* filter);
	};
}