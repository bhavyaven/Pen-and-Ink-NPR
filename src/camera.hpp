#pragma once

#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>

class Camera {
public:
	Camera();
	glm::mat4 getViewMatrix();
	void rotate(glm::vec2 delta);
	void zoom(float offset);

private:
	float yaw, pitch, dist;
};

#endif