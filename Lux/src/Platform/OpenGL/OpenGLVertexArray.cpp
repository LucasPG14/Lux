#include "luxpch.h"
#include "OpenGLVertexArray.h"

#include <glad/glad.h>

namespace Lux
{
	static GLenum ShaderTypeToOpenGL(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::FLOAT:		return GL_FLOAT;
		case ShaderDataType::FLOAT2:	return GL_FLOAT;
		case ShaderDataType::FLOAT3:	return GL_FLOAT;
		case ShaderDataType::FLOAT4:	return GL_FLOAT;
		case ShaderDataType::INT:	 	return GL_INT;
		case ShaderDataType::INT2: 		return GL_INT;
		case ShaderDataType::INT3:	 	return GL_INT;
		case ShaderDataType::INT4: 		return GL_INT;
		case ShaderDataType::MAT3: 		return GL_FLOAT;
		case ShaderDataType::MAT4:		return GL_FLOAT;
		}

		LUX_CORE_ASSERT(false, "ShaderDataType doesn't exist!");
		return 0;
	}

	OpenGLVertexArray::OpenGLVertexArray()
	{
		glCreateVertexArrays(1, &vertexArrayID);
	}
	
	OpenGLVertexArray::~OpenGLVertexArray()
	{
		glDeleteVertexArrays(1, &vertexArrayID);
	}
	
	void OpenGLVertexArray::Bind() const
	{
		glBindVertexArray(vertexArrayID);
	}
	
	void OpenGLVertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}
	
	void OpenGLVertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertex)
	{
		LUX_CORE_ASSERT(vertex->GetLayout().GetElements().size(), "Vertex Buffer hasn't layout!");
		glBindVertexArray(vertexArrayID);
		vertex->Bind();

		uint32_t i = 0;
		for (const auto& lay : vertex->GetLayout())
		{
			glEnableVertexAttribArray(i);
			glVertexAttribPointer(i, lay.GetCount(), ShaderTypeToOpenGL(lay.type), 
				lay.normalize ? GL_TRUE : GL_FALSE, vertex->GetLayout().GetStride(), (const void*)lay.offset);
			
			i++;
		}

		vertexBuffers.push_back(vertex);
	}
	
	void OpenGLVertexArray::AddIndexBuffer(const std::shared_ptr<IndexBuffer>& index)
	{
		glBindVertexArray(vertexArrayID);
		index->Bind();

		indexBuffer = index;
	}
}