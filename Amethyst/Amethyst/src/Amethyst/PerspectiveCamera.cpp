#include "amtpch.h"
#include "PerspectiveCamera.h"

#include "Application.h"
#include "Input.h"
#include "MouseCodes.h"
#include "KeyCodes.h"

#include <GLM/gtx/orthonormalize.hpp>

namespace Amethyst
{
	PerspectiveCamera::PerspectiveCamera() : viewMatrix(1.0f), projectionMatrix(1.0f), position(0.0f, 0.0f, 5.0f), up(0.0f, 1.0f, 0.0f),
		front(0.0f, 0.0f, -1.0f), hFov(glm::radians(70.0f)), vFov(0.0f), nearPlane(0.5f), farPlane(1000.0f), mouseInitialPos(0.0f)
	{
		Window& window = Application::Get().GetWindow();
		float aspectRatio = window.GetWidth() / window.GetHeight();
		vFov = 2 * glm::atan(glm::tan(hFov / 2) * aspectRatio);
		projectionMatrix = glm::perspective(vFov, aspectRatio, nearPlane, farPlane);
		viewMatrix = ComputeViewMatrix();
	}
	
	PerspectiveCamera::~PerspectiveCamera()
	{
	}
	
	bool PerspectiveCamera::Update(Timer timer)
	{
		glm::vec3 newPos = position;
		glm::vec3 newFront = front;
		glm::vec3 newUp = up;

		const glm::vec2& mouse = { Input::GetMouseX(), Input::GetMouseY() };
		glm::vec2 delta = (mouse - mouseInitialPos) * 0.0001f;
		mouseInitialPos = mouse;
		
		if (Input::IsMouseButtonPressed(Mouse::BUTTON_RIGHT))
		{
			if (Input::IsKeyPressed(Keys::W)) newPos += front * 0.025f;
			if (Input::IsKeyPressed(Keys::S)) newPos -= front * 0.025f;

			if (Input::IsKeyPressed(Keys::A)) newPos -= glm::cross(front, up) * 0.025f;
			if (Input::IsKeyPressed(Keys::D)) newPos += glm::cross(front, up) * 0.025f;

			if (Input::IsKeyPressed(Keys::Q)) newPos += up * 0.025f;
			if (Input::IsKeyPressed(Keys::E)) newPos -= up * 0.025f;

			if (delta.y != 0)
			{
				glm::quat quaternion = glm::quat(delta.y * timer.GetMilliSeconds(), glm::normalize(glm::cross(front, up)));
				newFront = quaternion * newFront * glm::conjugate(quaternion);
				newUp = quaternion * newUp * glm::conjugate(quaternion);
				glm::normalize(newFront);
				glm::normalize(newUp);
				glm::orthonormalize(newFront, newUp);
			}
			if (delta.x != 0)
			{
				glm::quat quaternion = glm::quat(delta.x * timer.GetMilliSeconds(), glm::vec3(0.0f, 1.0f, 0.0f));
				newFront = quaternion * newFront * glm::conjugate(quaternion);
				newUp = quaternion * newUp * glm::conjugate(quaternion);
				glm::normalize(newFront);
				glm::normalize(newUp);
				glm::orthonormalize(newFront, newUp);
			}

			position = newPos;
			up = newUp;
			front = newFront;
		}

		viewMatrix = ComputeViewMatrix();
		viewMatrix = glm::inverse(viewMatrix);

		return true;
	}
	
	void PerspectiveCamera::SetDimensions(float width, float height)
	{
		float aspectRatio = width / height;
		vFov = 2 * glm::atan(glm::tan(hFov / 2) * aspectRatio);
		projectionMatrix = glm::perspective(vFov, aspectRatio, nearPlane, farPlane);
	}
	
	glm::mat3x4 PerspectiveCamera::ComputeViewMatrix()
	{
		glm::vec3 worldRight = glm::normalize(glm::cross(front, up));
		glm::mat3x4 matrix =
		{
			worldRight.x, up.x, -front.x, position.x,
			worldRight.y, up.y, -front.y, position.y,
			worldRight.z, up.z, -front.z, position.z,
		};

		return matrix;
	}
}