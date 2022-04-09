#pragma once

#include "Amethyst/Utils/Vertex.h"

namespace Amethyst
{
	enum class ShaderDataType
	{
		NONE = 0,
		FLOAT,
		FLOAT2,
		FLOAT3,
		FLOAT4,
		INT,
		INT2,
		INT3,
		INT4,
		MAT3,
		MAT4
	};

	static uint32_t ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::FLOAT:		return 4;
		case ShaderDataType::FLOAT2:	return 8;
		case ShaderDataType::FLOAT3:	return 12;
		case ShaderDataType::FLOAT4:	return 16;
		case ShaderDataType::INT:		return 4;
		case ShaderDataType::INT2:		return 8;
		case ShaderDataType::INT3:		return 12;
		case ShaderDataType::INT4:		return 16;
		case ShaderDataType::MAT3:		return 36;
		case ShaderDataType::MAT4:		return 64;
		}

		AMT_CORE_ASSERT(false, "ShaderDataType doesn't exist!");
		return 0;
	}

	struct BufferElement
	{
		std::string name;
		ShaderDataType type;
		uint32_t size;
		uint32_t offset;
		bool normalize;

		BufferElement() : name(""), type(ShaderDataType::NONE), size(0), offset(0), normalize(false) {}

		BufferElement(const ShaderDataType elemType, const std::string& elemName, bool normalized = false)
			: name(elemName), type(elemType), size(ShaderDataTypeSize(elemType)), offset(0), normalize(normalized) {}

		uint32_t GetCount() const
		{
			switch (type)
			{
			case ShaderDataType::FLOAT:		return 1;
			case ShaderDataType::FLOAT2:	return 2;
			case ShaderDataType::FLOAT3:	return 3;
			case ShaderDataType::FLOAT4:	return 4;
			case ShaderDataType::INT:		return 1;
			case ShaderDataType::INT2:		return 2;
			case ShaderDataType::INT3:		return 3;
			case ShaderDataType::INT4:		return 4;
			case ShaderDataType::MAT3:		return 9;	// 3 * 3
			case ShaderDataType::MAT4:		return 16;  // 4 * 4
			}

			AMT_CORE_ASSERT(false, "ShaderDataType doesn't exist!");
			return 0;
		}

	};

	class BufferLayout
	{
	public:
		BufferLayout() : stride(0){}
		BufferLayout(const std::initializer_list<BufferElement>& elem) : elements(elem) 
		{
			CalculateOffset();
		}
		
		inline uint32_t GetStride() const { return stride; }
		inline const std::vector<BufferElement>& GetElements() const { return elements; }
		
		std::vector<BufferElement>::iterator begin() { return elements.begin(); }
		std::vector<BufferElement>::iterator end() { return elements.end(); }

		std::vector<BufferElement>::const_iterator begin() const { return elements.cbegin(); }
		std::vector<BufferElement>::const_iterator end() const { return elements.cend(); }

	private:
		void CalculateOffset()
		{
			uint32_t offset = 0;
			stride = 0;
			for (auto& element : elements)
			{
				element.offset = offset;
				offset += element.size;
				stride += element.size;
			}
		}

	private:
		std::vector<BufferElement> elements;
		uint32_t stride;
	};

	class VertexBuffer
	{
	public:
		virtual ~VertexBuffer() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual const BufferLayout& GetLayout() = 0;
		virtual void SetLayout(const BufferLayout& lay) = 0;

		static VertexBuffer* Create(float* vertices, uint32_t size);
		static VertexBuffer* Create(Vertex* vertices, uint32_t size);
	};

	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual uint32_t GetCount() const = 0;

		static IndexBuffer* Create(uint32_t* indices, uint32_t size);
	};
}