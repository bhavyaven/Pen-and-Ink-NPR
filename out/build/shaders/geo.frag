#version 330 core

//layout(location = 0) out vec3 outNormal;
//layout(location = 1) out vec3 outPos;

//in vec3 FragPos;
//in vec3 Normal;

//void main() {
    //outNormal = normalize(Normal) * 0.5 + 0.5;
    //outPos    = FragPos;
//}

in vec3 vNormalView;
out vec3 gNormal;

void main() {
    // Pack view-space normal into [0,1] for storage
    gNormal = normalize(vNormalView) * 0.5 + 0.5;
}