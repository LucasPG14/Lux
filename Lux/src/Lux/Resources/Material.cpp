#include "luxpch.h"
#include "Material.h"

namespace Lux
{
	Material::Material() : albedoColor(1.0f, 1.0f, 1.0f)
	{
		albedo = Texture2D::Create("Assets/Textures/BricksDiffuse.png");
		normal = Texture2D::Create("Assets/Textures/BricksNormal.png");
		metallic = Texture2D::Create("Assets/Textures/rustediron2_metallic.png");
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