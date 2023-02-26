#pragma once

#include "Texture2D.h"
#include "glm/glm.hpp"

namespace Lux
{
	class Material
	{
	public:
		Material();
		~Material();

		void Bind();

		glm::vec3& GetColor() { return albedoColor; }
		int GetType() { return type; }

		const std::shared_ptr<Texture2D>& GetDiffuse() {	return albedo; };
		const std::shared_ptr<Texture2D>& GetNormalMap() {	return normal; };
		const std::shared_ptr<Texture2D>& GetMetallic() {	return metallic; };
		const std::shared_ptr<Texture2D>& GetRoughness() { return roughness; };

	private:
		std::shared_ptr<Texture2D> albedo;
		glm::vec3 albedoColor;
		int type;

		std::shared_ptr<Texture2D> normal;
		std::shared_ptr<Texture2D> metallic;
		std::shared_ptr<Texture2D> roughness;

		friend class MaterialComponent;
	};
}