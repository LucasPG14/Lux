#include "luxpch.h"
#include "Material.h"

namespace Lux
{
	Material::Material() : albedoColor(1.0f, 1.0f, 1.0f)
	{
		albedo = Texture2D::Create("Assets/Textures/BricksDiffuse.png");
		normal = Texture2D::Create("Assets/Textures/BricksNormal.png");
	}

	Material::~Material()
	{

	}

	void Material::Bind()
	{
		albedo->Bind(0);
		normal->Bind(1);
	}
}