#pragma once

namespace Lux
{
	enum class FramebufferTextureFormat
	{
		NONE = 0,

		// Color
		RGBA8 = 1,
		RGBA16 = 2,

		// Red channel
		RED_INTEGER = 3,

		// Depth / Stencil
		DEPTH24_STENCIL8 = 4
	};

	struct FramebufferTextureSpecification
	{
		FramebufferTextureSpecification() = default;
		FramebufferTextureSpecification(FramebufferTextureFormat format)
			: textureFormat(format) {}

		FramebufferTextureFormat textureFormat = FramebufferTextureFormat::NONE;
	};

	struct FramebufferAttachmentSpecification
	{
		FramebufferAttachmentSpecification() = default;
		FramebufferAttachmentSpecification(std::initializer_list<FramebufferTextureSpecification> attach)
			: attachments(attach) {}

		std::vector<FramebufferTextureSpecification> attachments;
	};

	struct FramebufferSpecification
	{
		uint32_t width = 1280;
		uint32_t height = 720;

		uint32_t samples = 1;

		FramebufferAttachmentSpecification attachments;

		bool swapChainTarget = false;
	};

	class Framebuffer
	{
	public:
		virtual ~Framebuffer() {}

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual void BindTextures() = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;

		virtual uint32_t GetID() const = 0;

		virtual const FramebufferSpecification& GetSpec() const = 0;
		static std::shared_ptr<Framebuffer> Create(const FramebufferSpecification& specification);
	};
}