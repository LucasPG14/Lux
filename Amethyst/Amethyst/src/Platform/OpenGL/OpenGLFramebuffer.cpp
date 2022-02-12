#include "amtpch.h"
#include "OpenGLFramebuffer.h"

#include <glad/glad.h>

namespace Amethyst
{
	OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecification& specification) : spec(specification), framebufferID(0)
	{
		CreateFramebuffer();
	}
	
	OpenGLFramebuffer::~OpenGLFramebuffer()
	{
		glDeleteFramebuffers(1, &framebufferID);
		glDeleteTextures(1, &color);
		glDeleteTextures(1, &depth);
	}
	
	void OpenGLFramebuffer::CreateFramebuffer()
	{
		if (framebufferID)
		{
			glDeleteFramebuffers(1, &framebufferID);
			glDeleteTextures(1, &color);
			glDeleteTextures(1, &depth);
		}

		glCreateFramebuffers(1, &framebufferID);
		glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);

		glCreateTextures(GL_TEXTURE_2D, 1, &color);
		glBindTexture(GL_TEXTURE_2D, color);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, spec.width, spec.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color, 0);

		// ------- Cherno's Way----------------------------------------------------------------------
		glCreateTextures(GL_TEXTURE_2D, 1, &depth);
		glBindTexture(GL_TEXTURE_2D, depth);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, spec.width, spec.height);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, spec.width, spec.height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depth, 0);
		// ------------------------------------------------------------------------------------------

		//glCreateRenderbuffers(1, &depth);
		//glBindRenderbuffer(GL_RENDERBUFFER, depth);
		//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, spec.width, spec.height);
		//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth);

		AMT_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer isn't complete!");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFramebuffer::Resize(uint32_t width, uint32_t height)
	{
		spec.width = width;
		spec.height = height;
		CreateFramebuffer();
	}
	
	void OpenGLFramebuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);
		glViewport(0, 0, spec.width, spec.height);
	}
	
	void OpenGLFramebuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}