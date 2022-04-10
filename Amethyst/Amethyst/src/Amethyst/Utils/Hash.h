#pragma once

#include "amtpch.h"

namespace Amethyst
{
	template<typename Char>
	class HashString
	{
		static constexpr std::uint32_t Hashed(const Char* curr) noexcept
		{
			auto value = 2166136261;

			while (*curr != 0) 
			{
				value = (value ^ static_cast<std::uint32_t>(*(curr++))) * 16777619;
			}

			return value;
		}

	public:

		template<std::size_t N>
		static constexpr std::uint32_t value(const Char (&str)[N]) noexcept
		{
			return Hashed(str);
		}
	};

	using HashedString = HashString<char>;

	template<typename T, typename = void>
	struct TypeID
	{
		static constexpr std::uint32_t id() noexcept
		{
			auto value = HashedString::value(__FUNCSIG__);
			return value;
		}
	};
}