#pragma once

namespace Amethyst
{
	class Component;
	
	class Entity
	{
	public:
		Entity(const std::string& n = "Entity");
		~Entity();

		void Update();

		void DrawInspector();

		// Getters
		inline std::string& GetName() { return name; }

		template<typename T, typename... Args>
		T* CreateComponent(Args&&... args);

		void AddComponent(Component* component) { components.push_back(component); }

	private:
		std::string name;

		std::vector<Entity> childrens;
		std::vector<Component*> components;
	};
}