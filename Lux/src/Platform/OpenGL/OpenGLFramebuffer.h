#pragma once

#include "Lux/Renderer/Framebuffer.h"

namespace Lux
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

		void BindTextures() override;

		inline uint32_t GetID() const { return framebufferID; }

		inline const FramebufferSpecification& GetSpec() const override { return spec; }
	private:
		uint32_t framebufferID;
		FramebufferSpecification spec;

		std::vector<FramebufferTextureSpecification> colorSpecifications;
		FramebufferTextureSpecification depthSpecification;

		std::vector<uint32_t> colorAttachments;
		uint32_t depth;


		uint32_t positions;
		uint32_t normals;
		uint32_t colorSpecular;
	};
}