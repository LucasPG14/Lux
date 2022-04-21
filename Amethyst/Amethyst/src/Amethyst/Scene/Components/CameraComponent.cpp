#include "amtpch.h"
#include "CameraComponent.h"

#include "Amethyst/Core/Application.h"

#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>

namespace Amethyst
{
	CameraComponent::CameraComponent() : nearPlane(), farPlane(), hFov(glm::radians(70.0f)), viewMatrix(1.0f)
	{
		Window& window = Application::Get().GetWindow();
		float aspectRatio = (float)window.GetWidth() / (float)window.GetHeight();
		vFov = 2 * glm::atan(glm::tan(hFov / 2) * aspectRatio);
		projectionMatrix = glm::perspective(vFov, aspectRatio, nearPlane, farPlane);
	}
	
	CameraComponent::~CameraComponent()
	{
	}
	
	void CameraComponent::Update()
	{
	}
	
	void CameraComponent::DrawInspector()
	{
		if (ImGui::CollapsingHeader("Camera Component"))
		{
			ImGui::Text("Near Plane");
			ImGui::SameLine();
			ImGui::DragFloat("##NearPlane", &nearPlane);

			ImGui::Text("Far Plane");
			ImGui::SameLine();
			ImGui::DragFloat("##FarPlane", &farPlane);
		}
	}
}