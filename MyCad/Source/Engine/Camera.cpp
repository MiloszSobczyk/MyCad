#include "Camera.h"

#include "Camera.h"
#include <imgui/imgui.h>
#include <algorithm>

Algebra::Matrix4 Camera::GetTranslationMatrix()
{
	return Algebra::Matrix4::Translation(-position.x, -position.y, -position.z);
}

Algebra::Matrix4 Camera::GetZoomMatrix()
{
	return Matrix4(Vector4(zoom, zoom, zoom, 1.f));
}

Algebra::Matrix4 Camera::GetRotationMatrix()
{
	return Matrix4::RotationXByDegree(xAngle) * Matrix4::RotationYByDegree(yAngle) * Matrix4::RotationZByDegree(zAngle);
}

void Camera::HandleTranslation(const float& delta)
{
	if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle))
	{
		ImVec2 mosueDelta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Middle);

		Vector4 direction = Vector4(mosueDelta.x, -mosueDelta.y, 0, 0);
		if (direction.Length() > 0)
		{
			direction = direction.Normalize();
		}

		position = position + direction * 100.f * delta * GetRotationMatrix() * GetZoomMatrix();
	}
}

void Camera::HandleZoom(const float& dt)
{
	if (ImGui::GetIO().MouseWheel != 0.f)
	{
		zoom += ImGui::GetIO().MouseWheel * 0.1f;
		zoom = std::clamp(zoom, 0.1f, 5.0f);
	}
}

void Camera::HandleRotations(const float& dt)
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
		else if (yAngle < 0)
		{
			yAngle = 360;
		}

	}
}

Camera::Camera(Algebra::Vector4 position, float zoom)
	:position( position ), zoom( zoom ), yAngle( 0.f ), xAngle( 0.f ), zAngle( 0.f )
{
}

void Camera::HandleInput()
{
	const float delta = ImGui::GetIO().DeltaTime;
	HandleTranslation(delta);
	HandleZoom(delta);
	HandleRotations(delta);
}



Vector4 Camera::GetPosition()
{
	return position;
}

Matrix4 Camera::GetViewMatrix()
{
    return GetRotationMatrix() * GetTranslationMatrix() * GetZoomMatrix(); 
}
