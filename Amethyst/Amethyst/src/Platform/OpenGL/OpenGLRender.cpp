#include "amtpch.h"
#include "OpenGLRender.h"

#include <glad/glad.h>

namespace Amethyst
{
	void OpenGLRender::Init() const
	{
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	void OpenGLRender::ClearColor(const glm::vec4& color) const
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLRender::Clear() const
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRender::Draw(const std::shared_ptr<VertexArray>& vertexArray)
	{
		vertexArray->Bind();
		glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
		GLenum code = glGetError();
	}
}