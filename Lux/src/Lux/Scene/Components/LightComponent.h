#pragma once

#include "Component.h"

#include <glm/glm.hpp>

namespace Lux
{
	enum class LightType
	{
		DIRECTIONAL = 0,
		POINT = 1,
		SPOT
	};

	class LightComponent : public Component
	{
	public:
		LightComponent(LightType t = LightType::DIRECTIONAL);
		~LightComponent();

		void Update() override;

		void SetType(LightType t) { lightType = t; }
		void SetColor(glm::vec3 col) { color = col; }
		void SetIntensity(float r) { intensity = r; }

		inline const glm::vec3& GetColor() const { return color; }
		inline const float GetCutOff() const { return cutOff; }
		inline const float GetIntensity() const { return intensity; }

		inline glm::vec3& GetModifiedColor() { return color; }
		inline float& GetModifiedCutOff() { return cutOff; }
		inline float& GetModifiedIntensity() { return intensity; }

		inline const LightType& GetType() const { return lightType; }

		COMPONENT_TYPE(LIGHT)

	private:
		ComponentType type;
		glm::vec3 color;

		float cutOff;
		float intensity;

		LightType lightType;
	};
}