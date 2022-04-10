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

	private:
		std::shared_ptr<Material> material;
	};
}