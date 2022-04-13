#pragma once

#include "Resource.h"

namespace Amethyst
{
	class Texture2D : public Resource
	{
	public:
		virtual ~Texture2D() {}

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual uint32_t GetID() const = 0;

		virtual void Bind(uint32_t slot = 0) const = 0;
		virtual void Unbind(uint32_t slot = 0) const = 0;

		static std::shared_ptr<Texture2D> Create(const std::string& path);
	};
}