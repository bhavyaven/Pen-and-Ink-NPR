#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

uniform mat4 model, view, projection;

out vec3 vNormalWorld;
out vec3 vPosWorld;

void main() {
    vec4 worldPos = model * vec4(aPos, 1.0);
    vPosWorld     = worldPos.xyz;
    vNormalWorld  = mat3(transpose(inverse(model))) * aNormal;
    gl_Position   = projection * view * worldPos;
}


/*layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

out vec3 vNormalView;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    mat3 normalMatrix = transpose(inverse(mat3(view * model)));
    vNormalView = normalize(normalMatrix * aNormal);
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}*/