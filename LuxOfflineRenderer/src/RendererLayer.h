#pragma once

#include <Lux.h>

namespace Lux
{
	class RendererLayer : public Layer
	{
	public:
		RendererLayer();
		~RendererLayer();

		void OnCreate() override;
		void OnDestroy() override;

		void Update(Timer timer) override;
		void RenderImGui() override;

		void OnEvent(Event& e) override;

	private:
		void Render();

	private:
		std::shared_ptr<Texture2D> image;

		int viewportWidth;
		int viewportHeight;

		PerspectiveCamera camera;
	};
}