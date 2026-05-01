#include "mesh.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
using namespace std;
using namespace glm;

int indexOfNumberLetter(string& str, int offset);
int lastIndexOfNumberLetter(string& str);
vector<string> split(const string& s, char delim);

Mesh::Mesh(const std::string& path) {
	vao = 0;
	vbo = 0;
	ebo = 0;
	vcount = 0;
	load(path);
}

void Mesh::draw() {
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, vcount);
	glBindVertexArray(NULL);
}

void Mesh::load(const std::string& path) {
	if (vao) { glDeleteVertexArrays(1, &vao); vao = 0; }
	if (vbo) { glDeleteBuffers(1, &vbo); vbo = 0; }
	vcount = 0;

	std::ifstream file(path);
	if (!file.is_open()) {
		stringstream ss;
		ss << "Mesh::load() - Could not open file " << path;
		throw runtime_error(ss.str());
	}

	vector<vec3> raw_vertices;
	vector<vec3> raw_normals;
	vector<unsigned int> v_elements;
	vector<unsigned int> n_elements;

	string line;
	while (getline(file, line)) {
		if (line.substr(0, 2) == "v ") {
			int index1 = indexOfNumberLetter(line, 2);
			int index2 = lastIndexOfNumberLetter(line);
			vector<string> values = split(line.substr(index1, index2 - index1 + 1), ' ');

			if (values.size() >= 3) {
				vec3 vert(
					std::stof(values[0]),
					std::stof(values[1]),
					std::stof(values[2])
				);
				raw_vertices.push_back(vert);
			}
		}
		else if (line.substr(0, 3) == "vn ") {
			int index1 = indexOfNumberLetter(line, 2);
			int index2 = lastIndexOfNumberLetter(line);
			vector<string> values = split(line.substr(index1, index2 - index1 + 1), ' ');
			raw_normals.push_back(vec3(stof(values[0]), stof(values[1]), stof(values[2])));

		}
		else if (line.substr(0, 2) == "f ") {
			int index1 = indexOfNumberLetter(line, 2);
			int index2 = lastIndexOfNumberLetter(line);
			vector<string> values = split(line.substr(index1, index2 - index1 + 1), ' ');
			for (int i = 0; i < values.size() - 2; i++) {
				vector<string> v1 = split(values[0], '/');
				vector<string> v2 = split(values[i + 1], '/');
				vector<string> v3 = split(values[i + 2], '/');

				if (!v1.empty() && !v1[0].empty())
					v_elements.push_back(std::stoul(v1[0]) - 1);

				if (!v2.empty() && !v2[0].empty())
					v_elements.push_back(std::stoul(v2[0]) - 1);

				if (!v3.empty() && !v3[0].empty())
					v_elements.push_back(std::stoul(v3[0]) - 1);
				if (v1.size() >= 3 && v1[2].length() > 0) {
					n_elements.push_back(stoul(v1[2]) - 1);
					n_elements.push_back(stoul(v2[2]) - 1);
					n_elements.push_back(stoul(v3[2]) - 1);
				}
			}
		}
	}
	file.close();

	vector<vtx> vertices(v_elements.size());
	for (int i = 0; i < v_elements.size(); i += 3) {
		if (v_elements[i] >= raw_vertices.size()) {
			std::cout << "Invalid vertex index!\n";
			continue;
		}
		vertices[i + 0].pos = raw_vertices[v_elements[i + 0]];
		vertices[i + 1].pos = raw_vertices[v_elements[i + 1]];
		vertices[i + 2].pos = raw_vertices[v_elements[i + 2]];

		if (n_elements.size() > 0) {
			vertices[i + 0].norm = raw_normals[n_elements[i + 0]];
			vertices[i + 1].norm = raw_normals[n_elements[i + 1]];
			vertices[i + 2].norm = raw_normals[n_elements[i + 2]];
		}
		else {
			vec3 normal = normalize(cross(vertices[i + 1].pos - vertices[i + 0].pos,
				vertices[i + 2].pos - vertices[i + 0].pos));
			vertices[i + 0].norm = normal;
			vertices[i + 1].norm = normal;
			vertices[i + 2].norm = normal;
		}
	}
	vcount = vertices.size();

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vtx), vertices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vtx), NULL);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vtx), (GLvoid*)sizeof(vec3));

	glBindVertexArray(NULL);
	glBindBuffer(GL_ARRAY_BUFFER, NULL);
}

int indexOfNumberLetter(string& str, int offset) {
	for (int i = offset; i < str.length(); ++i) {
		if ((str[i] >= '0' && str[i] <= '9') || str[i] == '-' || str[i] == '.') return i;
	}
	return str.length();
}

int lastIndexOfNumberLetter(string& str) {
	for (int i = str.length() - 1; i >= 0; --i) {
		if ((str[i] >= '0' && str[i] <= '9') || str[i] == '-' || str[i] == '.') return i;
	}
	return 0;
}

vector<string> split(const string& s, char delim) {
	vector<string> elems;

	stringstream ss(s);
	string item;
	while (getline(ss, item, delim)) {
		elems.push_back(item);
	}

	return elems;
}


