#pragma once

namespace Amethyst
{
	class Component
	{
	public:
		Component() {}

		virtual ~Component() {}

		virtual void Update() = 0;

		virtual void DrawInspector() = 0;
	};
}