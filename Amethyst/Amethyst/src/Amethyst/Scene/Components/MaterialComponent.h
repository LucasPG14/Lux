#pragma once

#include "Component.h"

#include "Amethyst/Resources/Material.h"

namespace Amethyst
{
	class MaterialComponent : public Component
	{
	public:
		MaterialComponent(std::shared_ptr<Material> mat);

		~MaterialComponent();

		void Update() override;

		void DrawInspector() override;

		std::shared_ptr<Material> GetMaterial() { return material; }

	private:
		std::shared_ptr<Material> material;
	};
}