#include "luxpch.h"
#include "PerspectiveCamera.h"

#include "Core/Application.h"
#include "Core/Input.h"
#include "Core/MouseCodes.h"
#include "Core/KeyCodes.h"

//#include "Utils/Math.h"

#include <GLM/gtx/orthonormalize.hpp>

namespace Lux
{
	PerspectiveCamera::PerspectiveCamera() : viewMatrix(1.0f), projectionMatrix(1.0f), position(0.0f, 0.0f, 2.0f), up(0.0f, 1.0f, 0.0f),
		front(0.0f, 0.0f, -1.0f), hFov(glm::radians(70.0f)), vFov(0.0f), nearPlane(0.5f), farPlane(1000.0f), mouseInitialPos(0.0f)
	{
		Window& window = Application::Get().GetWindow();
		aspectRatio = (float)window.GetWidth() / (float)window.GetHeight();
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

		delta.x = -delta.x;
		delta.y = -delta.y;
		
		if (Input::IsMouseButtonPressed(Mouse::BUTTON_RIGHT))
		{
			if (Input::IsKeyPressed(Keys::W)) newPos += front * 0.025f;
			if (Input::IsKeyPressed(Keys::S)) newPos -= front * 0.025f;

			if (Input::IsKeyPressed(Keys::A)) newPos -= glm::normalize(glm::cross(front, up)) * 0.025f;
			if (Input::IsKeyPressed(Keys::D)) newPos += glm::normalize(glm::cross(front, up)) * 0.025f;

			if (Input::IsKeyPressed(Keys::Q)) newPos += up * 0.025f;
			if (Input::IsKeyPressed(Keys::E)) newPos -= up * 0.025f;

			if (delta.y != 0)
			{
				const glm::quat& quaternion = glm::quat(delta.y * timer.GetMilliSeconds(), glm::normalize(glm::cross(front, up)));
				const glm::quat& conjQuat = glm::conjugate(quaternion);

				newFront = glm::normalize(quaternion * newFront * conjQuat);
				newUp = glm::normalize(quaternion * newUp * conjQuat);
				newFront = glm::orthonormalize(newFront, newUp);
			}
			if (delta.x != 0)
			{
				const glm::quat& quaternion = glm::quat(delta.x * timer.GetMilliSeconds(), glm::vec3(0.0f, 1.0f, 0.0f));
				const glm::quat& conjQuat = glm::conjugate(quaternion);

				newFront = glm::normalize(quaternion * newFront * conjQuat);
				newUp = glm::normalize(quaternion * newUp * conjQuat);
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
		aspectRatio = width / height;
		vFov = 2 * glm::atan(glm::tan(hFov / 2) * aspectRatio);
		projectionMatrix = glm::perspective(vFov, aspectRatio, nearPlane, farPlane);
	}

	glm::vec3 PerspectiveCamera::NearPlanePos(glm::vec2& pos)
	{
		float nearPlaneWidth = tan(hFov * 0.5f) * nearPlane;
		float nearPlaneHeight = tan(vFov * 0.5f) * nearPlane;

		pos.x = pos.x * nearPlaneWidth;
		pos.y = pos.y * nearPlaneHeight;

		const glm::vec3& right = glm::cross(front, up);

		return position + front * nearPlane + pos.x * right + pos.y * up;
	}

	glm::vec3 PerspectiveCamera::FarPlanePos(glm::vec2& pos)
	{
		float farPlaneWidth = tan(hFov * 0.5f) * farPlane;
		float farPlaneHeight = tan(vFov * 0.5f) * farPlane;

		pos.x = pos.x * farPlaneWidth;
		pos.y = pos.y * farPlaneHeight;

		const glm::vec3& right = glm::cross(front, up);

		return position + (front * farPlane) + (pos.x * right) + (pos.y * up);
	}
	
	const glm::mat3x4 PerspectiveCamera::ComputeViewMatrix()
	{
		const glm::vec3& worldRight = glm::normalize(glm::cross(front, up));

		glm::mat3x4 matrix =
		{
			worldRight.x, worldRight.y, worldRight.z, position.x,
			up.x, up.y, up.z, position.y,
			-front.x, -front.y, -front.z, position.z,
		};

		matrix[0][3] = (worldRight.x * position.x) + (worldRight.y * position.y) + (worldRight.z * position.z);
		matrix[1][3] = (up.x * position.x) + (up.y * position.y) + (up.z * position.z);
		matrix[2][3] = (-front.x * position.x) + (-front.y * position.y) + (-front.z * position.z);

		return matrix;
	}
}