#pragma once

#include "Component.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glm/gtx/quaternion.hpp>

namespace Lux
{
	class TransformComponent : public Component
	{
	public:
		TransformComponent(glm::vec3 pos, glm::vec3 rot, glm::vec3 sca);

		~TransformComponent();

		void Update() override;

		// TODO: Review this, maybe there's another better option
		const glm::mat4 GetTransform()
		{
			const glm::mat4& rotationMat = glm::toMat4(glm::quat(glm::radians(rotation)));
			
			glm::mat4 mat = glm::translate(glm::mat4(1.0f), position) * rotationMat * glm::scale(glm::mat4(1.0f), scale);
			return mat;
		}

		// Setters
		inline void SetPosition(const glm::vec3& pos) { position = pos; }
		inline void SetRotation(const glm::vec3& rot) { rotation = rot; }
		inline void SetScale(const glm::vec3& sca) { scale = sca; }

		// Getters
		inline const glm::vec3& GetPosition() { return position; }
		inline const glm::vec3& GetRotation() { return rotation; }
		inline const glm::vec3& GetScale() { return scale; }

		// Getters
		inline glm::vec3& GetModifiedPosition() { return position; }
		inline glm::vec3& GetModifiedRotation() { return rotation; }
		inline glm::vec3& GetModifiedScale() { return scale; }

		COMPONENT_TYPE(TRANSFORM)

	private:
		ComponentType type;

		glm::vec3 position;
		glm::vec3 rotation;
		glm::vec3 scale;
	};
}