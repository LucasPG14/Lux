#include "luxpch.h"
#include "Material.h"

namespace Lux
{
	Material::Material(uint32_t newId) : id(newId), albedoColor(1.0f, 0.0f, 0.0f), type(0), refractionIndex(1.4f), metallic(0.5f), roughness(0.5f)
	{
		albedo = CreateSharedPtr<Texture2D>("Assets/Textures/rustediron2_basecolor.png");
		normal = CreateSharedPtr<Texture2D>("Assets/Textures/rustediron2_normal.png");
		metallicMap = CreateSharedPtr<Texture2D>("Assets/Textures/rustediron2_metallic.png");
		roughnessMap = CreateSharedPtr<Texture2D>("Assets/Textures/rustediron2_roughness.png");
	}

	Material::~Material()
	{

	}

	void Material::Bind()
	{
		albedo->Bind(0);
		normal->Bind(1);
		metallicMap->Bind(2);
		roughnessMap->Bind(3);
	}
}