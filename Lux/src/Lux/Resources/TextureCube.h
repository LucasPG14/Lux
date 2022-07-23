#pragma once

namespace Lux
{
	class TextureCube
	{
	public:
		TextureCube(const std::vector<std::string>& paths);
		virtual ~TextureCube();

		void Bind(uint32_t slot = 0) const;
		void Unbind(uint32_t slot = 0) const;

	private:
		uint32_t textureID;
	};
}