#pragma once

#include <xhash>

namespace Amethyst
{
	class UUID
	{
	public:
		UUID();
		UUID(const uint64_t id);

		operator uint64_t() const { return uuid; }
	private:
		uint64_t uuid;
	};
}

namespace std
{
	template<>
	struct hash<Amethyst::UUID>
	{
		std::size_t operator()(const Amethyst::UUID& uuid) const
		{
			return hash<uint64_t>()((uint64_t)uuid);
		}
	};
}