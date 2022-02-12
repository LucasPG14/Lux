#pragma once

namespace Amethyst
{
	struct FramebufferSpecification
	{
		uint32_t width = 1280;
		uint32_t height = 720;
		//FramebufferFormat
		uint32_t samples = 1;

		bool swapChainTarget = false;
	};

	class Framebuffer
	{
	public:
		virtual ~Framebuffer() {}

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;

		virtual uint32_t GetID() const = 0;

		virtual const FramebufferSpecification& GetSpec() const = 0;
		static std::shared_ptr<Framebuffer> Create(const FramebufferSpecification& specification);
	};
}