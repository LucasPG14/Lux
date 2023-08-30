#include "luxpch.h"
#include "Framebuffer.h"

#include "Renderer.h"

#include <stb_image_write.h>
#include <glad/glad.h>

namespace Lux
{
	namespace Utilities
	{
		static bool IsDepthFormat(FramebufferTextureFormat texFormat)
		{
			switch (texFormat)
			{
			case FramebufferTextureFormat::DEPTH24_STENCIL8: return true;
			}

			return false;
		}

		static void CreateTextures(uint32_t count, uint32_t* data)
		{
			glCreateTextures(GL_TEXTURE_2D, count, data);
		}

		static void AttachColorTexture(uint32_t id, GLenum internalFormat, GLenum format, GLenum type, uint32_t width, uint32_t height, int index)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, id, 0);
		}
	}

	Framebuffer::Framebuffer(const FramebufferSpecification& specification) : spec(specification), framebufferID(0)
	{
		for (FramebufferTextureSpecification texSpec : spec.attachments.attachments)
		{
			if (!Utilities::IsDepthFormat(texSpec.textureFormat))
				colorSpecifications.emplace_back(texSpec);
			else
				depthSpecification = texSpec;
		}

		Invalidate();
	}
	
	Framebuffer::~Framebuffer()
	{
		glDeleteFramebuffers(1, &framebufferID);
		glDeleteTextures(colorAttachments.size(), colorAttachments.data());
		glDeleteTextures(1, &depth);
	}

	void Framebuffer::Invalidate()
	{
		if (framebufferID)
		{
			glDeleteFramebuffers(1, &framebufferID);

			glDeleteTextures(colorAttachments.size(), colorAttachments.data());
			glDeleteTextures(1, &depth);
		}

		glCreateFramebuffers(1, &framebufferID);
		glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);

		// Attachments
		if (!colorSpecifications.empty())
		{
			colorAttachments.resize(colorSpecifications.size());
			Utilities::CreateTextures(colorAttachments.size(), colorAttachments.data());

			for (int i = 0; i < colorAttachments.size(); ++i)
			{
				glBindTexture(GL_TEXTURE_2D, colorAttachments[i]);

				switch (colorSpecifications[i].textureFormat)
				{
				case FramebufferTextureFormat::RGBA8:
					Utilities::AttachColorTexture(colorAttachments[i], GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, spec.width, spec.height, i);
					break;
				case FramebufferTextureFormat::RGBA16:
					Utilities::AttachColorTexture(colorAttachments[i], GL_RGBA16F, GL_RGBA, GL_FLOAT, spec.width, spec.height, i);
					break;
				}
			}
		}

		if (depthSpecification.textureFormat != FramebufferTextureFormat::NONE)
		{
			Utilities::CreateTextures(1, &depth);
			glBindTexture(GL_TEXTURE_2D, depth);
			glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, spec.width, spec.height);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depth, 0);
		}

		if (colorAttachments.size() > 1)
		{
			LUX_CORE_ASSERT(colorAttachments.size() <= 4, "There're too much color attachments");
			GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
			glDrawBuffers(colorAttachments.size(), buffers);
		}
		else if (colorAttachments.empty())
		{
			// Only depth pass
			glDrawBuffer(GL_NONE);
		}

		LUX_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer isn't complete!");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	
	void Framebuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);
		glViewport(0, 0, spec.width, spec.height);
	}
	
	void Framebuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	
	void Framebuffer::BindTextures(uint32_t offset)
	{
		for (int i = 0; i < colorAttachments.size(); ++i)
		{
			glActiveTexture(GL_TEXTURE0 + i + offset);
			glBindTexture(GL_TEXTURE_2D, colorAttachments[i]);
		}
	}

	void Framebuffer::UnbindTextures()
	{
		for (int i = 0; i < colorAttachments.size(); ++i)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}
	
	void Framebuffer::Resize(uint32_t width, uint32_t height)
	{
		LUX_CORE_TRACE("New framebuffer size: {0}, {1}", width, height);
		spec.width = width;
		spec.height = height;
		Invalidate();
	}

	void Framebuffer::SaveToFile(const std::string& path)
	{
		std::vector<unsigned char> data(spec.width * spec.height * 4);

		glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);

		glReadBuffer(GL_COLOR_ATTACHMENT0);
		glReadPixels(0, 0, spec.width, spec.height, GL_RGBA, GL_UNSIGNED_BYTE, data.data());

		stbi_flip_vertically_on_write(1);

		std::string p = path.find(".png") == std::string::npos ? path + ".png" : path;
		if (!stbi_write_png(p.c_str(), spec.width, spec.height, 4, data.data(), 0))
		{
			LUX_CORE_ERROR("Couldn't save the image");
		}

		Unbind();
	}
}