#pragma once
#include "../Libs/glm/glm/glm.hpp"
#include "../Libs/glm/glm/gtc/matrix_transform.hpp"
#include "InputClass.h"

class Camera
{
	//Functions
public:
	Camera(glm::vec3 pos, glm::vec3 target, glm::vec3 up, float screenWidth, float screenHeight);
	~Camera();
	void HandleInput(InputClass* input);
	inline glm::mat4 GetCameraProjection() { return m_cameraProjMat; };
	inline glm::mat4 GetCameraView() { return m_cameraViewMat; };
	inline glm::vec3 GetPosition() { return m_cameraPosition; };
	inline void SetNearPlane(float nearPlane) { m_near = nearPlane; };
	inline void SetFarPlane(float farPlane) { m_far = farPlane; };
	inline void SetFOV(float FOV) { m_FOV = FOV; };
	inline void SetCameraMoveEnable(bool onOff) { m_cameraMoveEnable = onOff; };
	
private:

	//Variables
public :

private:
	glm::mat4 m_cameraProjMat;	//Camera projection matrix
	glm::mat4 m_cameraViewMat;	//Camera view matrix

	glm::vec3 m_cameraFront;
	glm::vec3 m_cameraPosition;	//Camera Position
	glm::vec3 m_cameraTarget;	//Camera lookat
	glm::vec3 m_cameraUp;		//Camera up directional vector

	float m_FOV;

	float m_near;
	float m_far;

	float m_screenHeight;
	float m_screenWidth;

	float speed = 20.0f; // 3 units / second
	float mouseSpeed = 2.0f;
	// horizontal angle : toward -Z
	float horizontalAngle = 90.0f;
	// vertical angle : 0, look at the horizon
	float verticalAngle = 0.0f;

	bool m_cameraMoveEnable = false;
};

