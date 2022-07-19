#pragma once

#include <xhash>

namespace Lux
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
	struct hash<Lux::UUID>
	{
		std::size_t operator()(const Lux::UUID& uuid) const
		{
			return hash<uint64_t>()((uint64_t)uuid);
		}
	};
}