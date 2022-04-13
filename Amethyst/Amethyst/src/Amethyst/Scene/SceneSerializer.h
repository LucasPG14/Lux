#pragma once

#include "Scene.h"

namespace Amethyst
{
	class SceneSerializer
	{
	public:
		SceneSerializer(const std::shared_ptr<Scene>& sceneSer);

		void Serialize(const std::filesystem::path& path);
		
		bool Deserialize(const std::filesystem::path& path);

	private:
		std::shared_ptr<Scene> scene;
	};
}