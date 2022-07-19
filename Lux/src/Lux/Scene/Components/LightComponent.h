#pragma once

#include "Component.h"

#include <glm/glm.hpp>

namespace Lux
{
	enum class LightType
	{
		DIRECTIONAL = 0,
		POINT = 1
	};

	class LightComponent : public Component
	{
	public:
		LightComponent(LightType t = LightType::DIRECTIONAL);
		~LightComponent();

		void Update() override;

		void DrawInspector() override;

		inline const glm::vec3& GetColor() const { return color; }

		inline const LightType& GetType() const { return type; }

	private:
		glm::vec3 color;

		LightType type;
	};
}