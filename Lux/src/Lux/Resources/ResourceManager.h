#pragma once

namespace Lux
{
	class Material;
	class Texture2D;
	class Mesh;
	class UUID;
	struct Vertex;
	struct AABB;

	class ResourceManager
	{
	public:
		static void LoadFile(const std::filesystem::path& filepath);

		static void DestroyMaterial(const std::shared_ptr<Material>& material);

		static std::shared_ptr<Material> CreateMaterial();
		
		static const std::shared_ptr<Texture2D>& CreateTexture(const std::string& path);

		static const std::shared_ptr<Mesh>& CreateMesh(const AABB& aabb, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, const std::string& path, const std::string& origPath);

		static std::shared_ptr<Mesh> GetMesh(const std::string& path);
		
		static const std::shared_ptr<Texture2D>& GetTexture(const std::string& path);

	private:
		static std::vector<std::shared_ptr<Mesh>> meshes;

		static std::vector<std::shared_ptr<Material>> materials;
		static std::vector<std::shared_ptr<Texture2D>> textures;
		static uint32_t materialsCount;
	};
}