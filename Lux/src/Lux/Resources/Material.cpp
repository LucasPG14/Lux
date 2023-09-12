#include "luxpch.h"
#include "Material.h"

namespace Lux
{
	Material::Material(uint32_t newId) : id(newId), emission(false), absorption(0.0), albedoColor(1.0f, 1.0f, 1.0f), type(0), refractionIndex(1.5f), metallic(0.0f), roughness(0.5f), emissive(1.0, 1.0, 1.0), transmission(0.0)
	{
		diffuse = nullptr;
		normal = nullptr;
		metallicMap = nullptr;
		roughnessMap = nullptr;
	}

	Material::~Material()
	{

	}

	void Material::Bind()
	{
		diffuse->Bind(0);
		normal->Bind(1);
		metallicMap->Bind(2);
		roughnessMap->Bind(3);
	}
	
	void Material::SetID(int newID)
	{
		id = newID;
	}
	
	void Material::ResetIDTextures(int newID)
	{
		if (diffuse) diffuse->SetID(newID);
		if (normal) normal->SetID(newID);
		if (metallicMap) metallicMap->SetID(newID);
		if (roughnessMap) roughnessMap->SetID(newID);
	}
	
	void Material::SetIDTextures(std::vector<char>& textures, int newID)
	{
		if (diffuse && diffuse->GetImageID() == -1)
		{
			diffuse->SetID(newID++);
			diffuse->Add(textures);
		}
		if (normal && normal->GetImageID() == -1)
		{
			normal->SetID(newID++);
			normal->Add(textures);
		}
		if (metallicMap && metallicMap->GetImageID() == -1)
		{
			metallicMap->SetID(newID++);
			metallicMap->Add(textures);
		}
		if (roughnessMap && roughnessMap->GetImageID() == -1)
		{
			roughnessMap->SetID(newID++);
			roughnessMap->Add(textures);
		}
	}
}