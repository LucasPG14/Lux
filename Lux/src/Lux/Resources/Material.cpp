#include "luxpch.h"
#include "Material.h"

namespace Lux
{
	Material::Material() : albedoColor(1.0f, 1.0f, 1.0f)
	{
		albedo = CreateSharedPtr<Texture2D>("Assets/Textures/BricksDiffuse.png");
		normal = CreateSharedPtr<Texture2D>("Assets/Textures/BricksNormal.png");
		metallic = CreateSharedPtr<Texture2D>("Assets/Textures/rustediron2_metallic.png");
		//roughness = Texture2D::Create("Assets/Textures/rustediron2_roughness.png");
	}

	Material::~Material()
	{

	}

	void Material::Bind()
	{
		albedo->Bind(0);
		normal->Bind(1);
		metallic->Bind(2);
		//roughness->Bind(3);
	}
}