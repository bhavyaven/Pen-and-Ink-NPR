#version 330 core

layout(location = 0) out vec3 gNormal;
layout(location = 1) out vec3 gPosition;

in vec3 vNormalWorld;
in vec3 vPosWorld;

void main() {
    gNormal   = normalize(vNormalWorld) * 0.5 + 0.5; // encode to [0,1]
    gPosition = vPosWorld;
}