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

		// This should not be here, needs to be removed
		void Update();

		void Load() override;

		void UnLoad() override;

	private:
		std::shared_ptr<VertexArray> vao;
	};
}