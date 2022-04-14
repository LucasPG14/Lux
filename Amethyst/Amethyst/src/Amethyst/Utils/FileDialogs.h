#pragma once

namespace Amethyst
{
	//// If the engine supports more than one system, this needs to change
	std::string OpenFile(const char* filter);

	std::string SaveFile(const char* filter);
}