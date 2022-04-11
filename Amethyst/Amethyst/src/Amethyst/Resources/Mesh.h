#pragma once

#include "Resource.h"

#include "Amethyst/Renderer/VertexArray.h"

namespace Amethyst
{
	class Mesh : public Resource
	{
	public:
		Mesh(std::filesystem::path& filePath);

		~Mesh();

		void Load() override;

		void UnLoad() override;

		const std::shared_ptr<VertexArray>& Get() { return vao; }

	private:
		std::shared_ptr<VertexArray> vao;
	};
}