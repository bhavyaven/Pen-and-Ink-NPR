#pragma once

#ifndef SHADER_HPP
#define SHADER_HPP

#include <../include/glad/include/glad/glad.h>
#include <glm/glm.hpp>
#include <string>

class Shader {
public:
	GLuint ID;

	Shader(const char* vert, const char* frag);
	void use();
	void setUniform(const std::string& name, const glm::mat4& mat) const;
};

#endif