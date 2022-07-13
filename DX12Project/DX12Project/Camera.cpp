#include "Camera.h"
#include "glm/ext.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include <Windows.h>
Camera::Camera(glm::vec3 pos, glm::vec3 target, glm::vec3 up, float screenWidth, float screenHeight)
{
	m_cameraPosition = pos;
	m_cameraTarget = target;
	m_cameraUp = up;
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;
	m_near = 0.1f;
	m_far = 1000.0f;
	m_FOV = 45.0f;
	m_cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);

	m_cameraViewMat = glm::lookAtRH(m_cameraPosition, m_cameraPosition + m_cameraFront, m_cameraUp);
	m_cameraProjMat = glm::perspectiveFovRH_ZO(glm::radians(m_FOV), (float)m_screenWidth, (float)m_screenHeight, m_near, m_far);
}

Camera::~Camera()
{
}

void Camera::HandleInput(InputClass* input)
{
	if (input->IsKeyDown(VK_SPACE))
	{
		// Get mouse position
		double xpos, ypos;
		POINT point;
		GetCursorPos(&point);
		xpos = point.x;
		ypos = point.y;
		SetCursorPos(m_screenWidth / 2, m_screenHeight / 2);
		//glfwGetCursorPos(window, &xpos, &ypos);
		//glfwSetCursorPos(window, m_screenWidth / 2, m_screenHeight / 2);
		//xpos = m_screenWidth / 2;
		//ypos = m_screenWidth / 2;
		float xoffSet = ((float)xpos - (m_screenWidth / 2)) * mouseSpeed * 0.016f;
		float yoffSet = ((float)ypos - (m_screenHeight / 2)) * mouseSpeed * 0.016f;


		horizontalAngle += xoffSet;
		verticalAngle += yoffSet;

		// Compute new orientation
		//Make srue that wehn the pitch is out of bounds screen doesn't get flipped
		//if (verticalAngle > 89.0f)
		//	verticalAngle = 89.0f;
		//if (verticalAngle < -89.0f)
		//	verticalAngle = -89.0f;

		//horizontalAngle += mouseSpeed* 0.016f * float(m_screenWidth / 2 - xpos);
		//verticalAngle += mouseSpeed * 0.016f * float(m_screenHeight / 2 - ypos);

		//glm::vec3 direction(0.0f, 0.0f, -1.0f);
		glm::vec3 up(0.0f, 1.0f, 0.0f);
		//glm::vec3 direction(
		//	cos(glm::radians(verticalAngle)) * cos(glm::radians(horizontalAngle)),
		//	sin(glm::radians(horizontalAngle)),
		//	sin(glm::radians(verticalAngle)) * cos(glm::radians(horizontalAngle))
		//);

		glm::vec3 direction(
			cos(glm::radians(horizontalAngle)) * cos(glm::radians(verticalAngle)),
			sin(glm::radians(verticalAngle)),
			sin(glm::radians(horizontalAngle)) * cos(glm::radians(verticalAngle))
		);
		direction = glm::normalize(direction);
		m_cameraFront = direction;

		//
		//// Right vector
		glm::vec3 right = glm::cross(direction, up);
		//glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);
		//glm::vec3 right(sin(horizontalAngle - 3.14f/2.0f),0,cos(horizontalAngle - 3.14f/2.0f));

		// Up vector : perpendicular to both direction and right

		//glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f);

		// Move forward
		if (input->IsKeyDown(0x57)) {
			m_cameraPosition += direction * 0.016f * speed;
		}
		// Move backward
		if (input->IsKeyDown(0x53)) {
			m_cameraPosition -= direction * 0.016f * speed;
		}
		// Strafe right
		if (input->IsKeyDown(0x44)) {
			m_cameraPosition += right * 0.016f * speed;
		}
		// Strafe left
		if (input->IsKeyDown(0x41)) {
			m_cameraPosition -= right * 0.016f * speed;
		}

		m_cameraViewMat = glm::lookAtRH(m_cameraPosition, m_cameraPosition + m_cameraFront, m_cameraUp);
		m_cameraProjMat = glm::perspectiveFovRH_ZO(glm::radians(m_FOV), (float)m_screenWidth, (float)m_screenHeight, m_near, m_far);
	}

	//if (input->IsKeyDown(VK_SPACE))
	//{
	//	m_cameraMoveEnable = !m_cameraMoveEnable;
	//}
}

