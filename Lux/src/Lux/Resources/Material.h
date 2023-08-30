#pragma once

#include "Texture2D.h"
#include "glm/glm.hpp"

namespace Lux
{
	class Material
	{
	public:
		Material(uint32_t newId);
		~Material();

		void Bind();

		glm::vec4& GetColor()		{ return albedoColor; }
		glm::vec4& GetEmissive()		{ return emissive; }
		float& GetMetallic()			{ return metallic; }
		float& GetRoughness()		{ return roughness; }
		float& GetRefractionIndex()	{ return refractionIndex; }
		float& GetTransmission()	{ return transmission; }

		int GetType() { return type; }

		int GetID() { return id; }
		void SetID(int newID = -1) { id = newID; }

		const std::shared_ptr<Texture2D>& GetDiffuse()		{ return albedo; };
		const std::shared_ptr<Texture2D>& GetNormalMap()	{ return normal; };
		const std::shared_ptr<Texture2D>& GetMetallicMap()	{ return metallicMap; };
		const std::shared_ptr<Texture2D>& GetRoughnessMap() { return roughnessMap; };

		void SetDiffuse(const std::shared_ptr<Texture2D>& texture) { albedo = texture; };
		void SetNormalMap(const std::shared_ptr<Texture2D>& texture) { normal = texture; };
		void SetMetallicMap(const std::shared_ptr<Texture2D>& texture) { metallicMap = texture; };
		void SetRoughnessMap(const std::shared_ptr<Texture2D>& texture) { roughnessMap = texture; };

		void SetColor(const glm::vec4& col) { albedoColor = col; }
		void SetMetallic(float met) { metallic = met; }
		void SetRoughness(float rough) { roughness = rough; }
		void SetRefractionIndex(float idx) { refractionIndex = idx; }
		void SetTransmission(float idx) { transmission = idx; }

	private:
		int id;

		std::shared_ptr<Texture2D> albedo;
		glm::vec4 albedoColor;
		glm::vec4 emissive;
		int type;

		std::shared_ptr<Texture2D> normal;
		std::shared_ptr<Texture2D> metallicMap;
		std::shared_ptr<Texture2D> roughnessMap;

		float refractionIndex;
		float metallic;
		float roughness;
		float transmission;

		friend class MaterialComponent;
	};
}