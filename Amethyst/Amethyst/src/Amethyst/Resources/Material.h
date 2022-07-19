#pragma once

#include "Texture.h"
#include "glm/glm.hpp"

namespace Amethyst
{
	class Material
	{
	public:
		Material();
		~Material();

		void Bind();

		glm::vec3& GetColor() { return albedoColor; }

	private:
		std::shared_ptr<Texture2D> albedo;
		glm::vec3 albedoColor;

		std::shared_ptr<Texture2D> normal;
		std::shared_ptr<Texture2D> metallic;


		friend class MaterialComponent;
	};
}