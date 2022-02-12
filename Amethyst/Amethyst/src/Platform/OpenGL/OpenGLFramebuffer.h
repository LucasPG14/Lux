#pragma once

#include "Amethyst/Renderer/Framebuffer.h"

namespace Amethyst
{
	class OpenGLFramebuffer : public Framebuffer
	{
	public:
		OpenGLFramebuffer(const FramebufferSpecification& specification);
		virtual ~OpenGLFramebuffer();

		void CreateFramebuffer();

		void Resize(uint32_t width, uint32_t height);

		void Bind() override;
		void Unbind() override;

		inline uint32_t GetID() const { return framebufferID; }

		inline const FramebufferSpecification& GetSpec() const override { return spec; }
	private:
		uint32_t framebufferID;
		uint32_t color;
		uint32_t depth;
		FramebufferSpecification spec;
	};
}