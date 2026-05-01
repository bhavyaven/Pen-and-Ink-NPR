#include "camera.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

Camera::Camera() {
	yaw = 0.0f;
	pitch = 0.0f;
	dist = 5.0f;
}

glm::mat4 Camera::getViewMatrix() {
	glm::vec3 pos;
	pos.x = dist * cos(glm::radians(pitch)) * sin(glm::radians(yaw));
	pos.y = dist * sin(glm::radians(pitch));
	pos.z = dist * cos(glm::radians(pitch)) * cos(glm::radians(yaw));
	
	return glm::lookAt(pos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}

void Camera::rotate(glm::vec2 delta) {
	float sense = 0.3f;
	yaw += delta.x * sense;
	pitch += delta.y * sense;
	pitch = glm::clamp(pitch, -89.0f, 89.0f);
}

void Camera::zoom(float offset) {
	dist += offset;
	dist = glm::clamp(dist, 0.5f, 30.0f);
}