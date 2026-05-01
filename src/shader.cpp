#include "shader.hpp"
#include <iostream>
#include <sstream>
#include <fstream>
using namespace std;

Shader::Shader(const char* vert, const char* frag) {
	std::ifstream test(vert);
	if (!test.good())
		std::cerr << "WARNING: shader file not found: " << vert << "\n";

	test.close();
	test.open(frag);
	if (!test.good())
		std::cerr << "WARNING: shader file not found: " << frag << "\n";


	std::ifstream vfile(vert);
	if (!vfile.is_open()) {
		std::cerr << "Could not open " << vert << "!" << endl;
		exit(-1);
	}

	std::ifstream ffile(frag);
	if (!ffile.is_open()) {
		std::cerr << "Could not open " << frag << "!" << endl;
		exit(-1);
	}

	std::stringstream vbuff;
	vbuff << vfile.rdbuf();
	std::string vcode = vbuff.str();

	std::stringstream fbuff;
	fbuff << ffile.rdbuf();
	std::string fcode = fbuff.str();

	auto stripBOM = [](std::string& s) {
		if (s.size() >= 3 &&
			(unsigned char)s[0] == 0xEF &&
			(unsigned char)s[1] == 0xBB &&
			(unsigned char)s[2] == 0xBF)
			s = s.substr(3);
		};
	stripBOM(vcode);
	stripBOM(fcode);

	const char* vsrc = vcode.c_str();
	const char* fsrc = fcode.c_str();

	GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertShader, 1, &vsrc, nullptr);
	glCompileShader(vertShader);

	GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragShader, 1, &fsrc, nullptr);
	glCompileShader(fragShader);

	GLint success;
	glGetShaderiv(vertShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char log[512];
		glGetShaderInfoLog(vertShader, 512, NULL, log);
		std::cout << "Shader compile error:\n" << log << std::endl;
		exit(-1);
	}

	glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char log[512];
		glGetShaderInfoLog(fragShader, 512, NULL, log);
		std::cout << "Shader compile error:\n" << log << std::endl;
		exit(-1);
	}

	ID = glCreateProgram();
	glAttachShader(ID, vertShader);
	glAttachShader(ID, fragShader);
	glLinkProgram(ID);

	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success) {
		char log[512];
		glGetProgramInfoLog(ID, 512, NULL, log);
		std::cout << "Program link error:\n" << log << std::endl;
		exit(-1);
	}

	glDeleteShader(vertShader);
	glDeleteShader(fragShader);
}

void Shader::use() {
	glUseProgram(ID);
}

void Shader::setUniform(const std::string& name, const glm::mat4& mat) const {
	GLint loc = glGetUniformLocation(ID, name.c_str());
	glUniformMatrix4fv(loc, 1, GL_FALSE, &mat[0][0]);
}