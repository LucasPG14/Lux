#pragma once

#include "Component.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glm/gtx/quaternion.hpp>

namespace Amethyst
{
	class TransformComponent : public Component
	{
	public:
		TransformComponent(glm::vec3 pos, glm::vec3 rot, glm::vec3 sca);

		~TransformComponent();

		void Update() override;

		void DrawInspector() override;

		// TODO: Review this, maybe there's another better option
		const glm::mat4 GetTransform()
		{
			glm::mat4& rotationMat = glm::toMat4(glm::quat(glm::radians(rotation)));
			glm::mat4& transform = glm::translate(glm::mat4(1.0f), position);
			glm::mat4& scaleMat = glm::scale(glm::mat4(1.0f), scale);
			
			return glm::transpose(transform * rotationMat * scaleMat);
		}

		inline void SetPosition(glm::vec3& pos) { position = pos; }
		inline void SetDeltaRotation(glm::vec3& rot) { rotation = rot; }
		inline void SetScale(glm::vec3& sca) { scale = sca; }

	private:
		glm::vec3 position;
		glm::vec3 rotation;
		glm::vec3 scale;
	};
}