#include "Camera.h"

#include <imgui/imgui.h>
#include <algorithm>

Algebra::Matrix4 Camera::GetTranslationMatrix()
{
	return Algebra::Matrix4::Translation(-position.x, -position.y, -position.z);
}

Algebra::Matrix4 Camera::GetZoomMatrix()
{
	return Algebra::Matrix4::Scale(zoom, zoom, zoom);
}

Algebra::Matrix4 Camera::GetRotationMatrix()
{
	return Algebra::Matrix4::RotationXByDegree(xAngle) * Algebra::Matrix4::RotationYDegree(yAngle) * Algebra::Matrix4::RotationZDegree(zAngle);
}

void Camera::HandleTranslation(const float& deltaTime)
{
	if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle))
	{
		ImVec2 delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Middle);

		Algebra::Vector4 direction = Algebra::Vector4(delta.x, -delta.y, 0, 0);
		if (direction.Length() > 0)
		{
			direction = direction.Normalize();
		}

		position += direction * 100.f * deltaTime * GetRotationMatrix() * GetZoomMatrix();
	}
}

void Camera::HandleZoom(const float& deltaTime)
{
	if (ImGui::GetIO().MouseWheel != 0.f)
	{
		zoom += ImGui::GetIO().MouseWheel * 0.1f;
		zoom = std::clamp(zoom, 0.1f, 5.0f);
	}
}

void Camera::HandleRotations(const float& deltaTime)
{
	if (ImGui::IsMouseDragging(ImGuiMouseButton_Right))
	{
		if (ImGui::GetIO().KeyShift)
		{
			ImVec2 delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);
			zAngle += delta.x / 10.f;
		}
		else
		{
			ImVec2 delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);
			yAngle += delta.x / 10.f;
			xAngle += delta.y / 10.f;
		}
		
		ImGui::ResetMouseDragDelta(ImGuiMouseButton_Right);

		xAngle = std::clamp(xAngle, -90.f, 90.f);
		zAngle = std::clamp(zAngle, -90.f, 90.f);
		if (yAngle > 360)
		{
			yAngle = 0;
		}
		else if(yAngle < 0)
		{
			yAngle = 360;
		}
		
	}
}

Camera::Camera(Algebra::Vector4 position, float zoom)
	: position(position), zoom(zoom), yAngle(0.f), xAngle(0.f), zAngle(0.f)
{
}

Algebra::Vector4 Camera::GetPosition() 
{
	return position; 
}

Algebra::Matrix4 Camera::GetViewMatrix() 
{ 
	return GetRotationMatrix() * GetTranslationMatrix() * GetZoomMatrix(); 
}

void Camera::HandleInput()
{
	const float deltaTime = ImGui::GetIO().DeltaTime;
	HandleTranslation(deltaTime);
	HandleZoom(deltaTime);
	HandleRotations(deltaTime);
}


