#pragma once

#include "Resource.h"

#include "Amethyst/Resources/Texture.h"

namespace Amethyst
{
	class Material : public Resource
	{
	public:
		Material(UUID id, const std::filesystem::path& filePath);

		~Material();

		void Load() override;

		void UnLoad() override;

		void Bind();

		void Unbind();

		const std::shared_ptr<Texture2D>& GetDiffuse() { return diffuse; }

	private:
		std::shared_ptr<Texture2D> diffuse;
		//Texture2D* normal;
	};
}