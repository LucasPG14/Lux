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

		inline const glm::vec3& GetColor() const { return color; }

		inline glm::vec3& GetModifiedColor() { return color; }

		inline const LightType& GetType() const { return lightType; }

		COMPONENT_TYPE(LIGHT)

	private:
		ComponentType type;
		glm::vec3 color;

		LightType lightType;
	};
}