#pragma once

#include "Lux/Core/UUID.h"

namespace Lux
{
	class Component;
	
	class Entity
	{
	public:
		Entity(UUID id, const std::string& n = "Entity");
		~Entity();

		void Update();

		// Getters
		inline const std::string& GetName() const { return name; }
		inline std::string& GetModifiedName() { return name; }
		inline const UUID& GetUUID() const { return uuid; }

		template<typename T, typename... Args>
		T* CreateComponent(Args&&... args)
		{
			T* component = new T(std::forward<Args>(args)...);
			components.push_back(component);
			return component;
		}

		template<typename T>
		T* Get()
		{
			for (std::vector<Component*>::iterator i = components.begin(); i != components.end(); ++i)
			{
				//component = dynamic_cast<T*>(*i);
				//if (component != nullptr)
				//	return component;
				if ((*i)->GetComponentType() == T::GetStaticType())
					return (T*)(*i);
			}

			return nullptr;
		}

		void AddComponent(Component* component) { components.push_back(component); }

		void Destroy();

		operator bool() const { return name != ""; }
	private:
		UUID uuid;
		std::string name;

		std::vector<Entity> childrens;
		std::vector<Component*> components;
	};
}