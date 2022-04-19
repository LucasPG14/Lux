#include "amtpch.h"
#include "OpenGLRender.h"

#include <glad/glad.h>

namespace Amethyst
{
	void OpenGLRender::Init() const
	{
		glEnable(GL_TEXTURE_2D);
		/*glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);*/
		glEnable(GL_DEPTH_TEST);
	}

	void OpenGLRender::ClearColor(const glm::vec4& color) const
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLRender::Clear() const
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRender::ChangeState(bool change) const
	{
		if (change) glDisable(GL_CULL_FACE);
		else glEnable(GL_CULL_FACE);
	}

	void OpenGLRender::Draw(const std::shared_ptr<VertexArray>& vertexArray, uint32_t indexCount)
	{
		uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
		GLenum code = glGetError();
		bool ret = true;
		ret = false;
	}
}