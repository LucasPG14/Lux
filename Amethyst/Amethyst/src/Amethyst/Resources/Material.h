#pragma once

#include "Resource.h"

//#include "Amethyst/Renderer/Texture.h"

namespace Amethyst
{
	class Material : public Resource
	{
	public:
		Material(std::filesystem::path& filePath);

		~Material();

		void Load() override;

		void UnLoad() override;

	private:
		//Texture2D* diffuse;
		//Texture2D* normal;
	};
}