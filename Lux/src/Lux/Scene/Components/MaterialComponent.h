#pragma once

#include "Component.h"
#include "Lux/Resources/Material.h"

namespace Lux
{
	class MaterialComponent : public Component
	{
	public:
		MaterialComponent();

		~MaterialComponent();

		void Update() override;

		const std::shared_ptr<Material>& GetMaterial() { return material; }

		ComponentType GetType() { return type; }
		COMPONENT_TYPE(MATERIAL)

	private:
		ComponentType type;

		std::shared_ptr<Material> material;
	};
}