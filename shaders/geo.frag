#version 330 core

in vec3 vNormalView;
out vec3 gNormal;

void main() {
    gNormal = normalize(vNormalView) * 0.5 + 0.5;
}