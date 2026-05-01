#pragma once

#ifndef MESH_HPP
#define MESH_HPP

#include <string>
#include <vector>
#include <../include/glad/include/glad/glad.h>
#include <glm/glm.hpp>

class Mesh {
public:
	Mesh(const std::string& path);
	void load(const std::string& path);
	void draw();

	struct vtx {
		glm::vec3 pos;
		glm::vec3 norm;
	};

private:
	unsigned int vao;
	unsigned int vbo;
	unsigned int ebo;
	unsigned int vcount;
};

#endif