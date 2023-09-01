#pragma once

namespace Lux
{
	class Material;

	class Texture2DArray
	{
	public:
		Texture2DArray(const std::shared_ptr<Material>& material);
		Texture2DArray(const std::string& filename);
		Texture2DArray(uint32_t size, void* data);
		~Texture2DArray();

		void AddMaterial(const std::shared_ptr<Material>& material);
		void AddTexture(const std::string& filename);

		void Bind(uint32_t slot);

	private:
		uint32_t textureID;
	};
}