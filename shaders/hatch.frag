#version 330 core

in vec2 vUV;
out float fragHatch;

uniform sampler2D normalTex;
uniform sampler2D posTex;
uniform sampler2D hatchTex;
uniform float uHatchScale;
uniform int   uIsInk;

float sampleHatch(vec2 uv) {
    return texture(hatchTex, uv * uHatchScale).r;
}

void main() {
    vec3 rawNormal = texture(normalTex, vUV).rgb;

    float grayDist = length(rawNormal - vec3(0.5));
    if (grayDist < 0.05) {
        fragHatch = 0.0;
        return;
    }

    vec3 normal = normalize(rawNormal * 2.0 - 1.0);
    vec3 worldPos = texture(posTex, vUV).rgb;

    vec3 blend = pow(abs(normal), vec3(4.0));
    blend /= (blend.x + blend.y + blend.z + 1e-5);

    float hatchXY = sampleHatch(worldPos.xy); 
    float hatchXZ = sampleHatch(worldPos.xz); 
    float hatchYZ = sampleHatch(worldPos.yz); 

    float texSample = hatchXY * blend.z
                    + hatchXZ * blend.y
                    + hatchYZ * blend.x;

    vec3  lightDir = normalize(vec3(0.5, 1.0, 0.8));
    float NdotL    = clamp(dot(normal, lightDir), 0.0, 1.0);
    float darkness = 1.0 - NdotL;

    float stroke;
    if (uIsInk == 1) {
        stroke = (1.0 - texSample) * darkness;
    } else {
        stroke = (1.0 - texSample) * darkness;
    }

    fragHatch = clamp(stroke, 0.0, 1.0);
}