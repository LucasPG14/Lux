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
		Framebuffer(const FramebufferSpecification& specification);
		virtual ~Framebuffer();

		void Invalidate();

		void Bind();
		void Unbind();

		void BindTextures(uint32_t offset = 0);
		void UnbindTextures();

		void Resize(uint32_t width, uint32_t height);

		void SaveToFile(const std::string& path);

		uint32_t GetID() const { return colorAttachments[0]; }

		const FramebufferSpecification& GetSpec() const { return spec; }

	private:

		uint32_t framebufferID;
		FramebufferSpecification spec;

		std::vector<FramebufferTextureSpecification> colorSpecifications;
		FramebufferTextureSpecification depthSpecification;

		std::vector<uint32_t> colorAttachments;
		uint32_t depth;
	};
}