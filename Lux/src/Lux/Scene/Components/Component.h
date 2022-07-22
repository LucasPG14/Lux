#pragma once

namespace Lux
{
	enum class ComponentType
	{
		TRANSFORM = 0,
		MESH = 1,
		MATERIAL = 2,
		LIGHT = 3
	};

#define COMPONENT_TYPE(type) static ComponentType GetStaticType() { return ComponentType::##type; }\
							 virtual ComponentType GetComponentType() const override { return GetStaticType(); }

	class Component
	{
	public:
		virtual ~Component() {}

		virtual void Update() = 0;

		virtual void DrawInspector() = 0;

		virtual ComponentType GetComponentType() const = 0;
	};
}