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

    // Background check — encoded 0.5,0.5,0.5 grey means no geometry
    float grayDist = length(rawNormal - vec3(0.5));
    if (grayDist < 0.05) {
        fragHatch = 0.0;
        return;
    }

    vec3 normal = normalize(rawNormal * 2.0 - 1.0);
    vec3 worldPos = texture(posTex, vUV).rgb;

    // ── Triplanar blend weights ──────────────────────────────────────────────
    // Raise abs(normal) to a power to sharpen the blend seams.
    // Power 4 gives tight blending; lower values (e.g. 2) give softer crossfades.
    vec3 blend = pow(abs(normal), vec3(4.0));
    blend /= (blend.x + blend.y + blend.z + 1e-5); // normalise to sum = 1

    // ── Sample hatch texture from three axes ─────────────────────────────────
    float hatchXY = sampleHatch(worldPos.xy); // top/bottom faces
    float hatchXZ = sampleHatch(worldPos.xz); // front/back faces
    float hatchYZ = sampleHatch(worldPos.yz); // left/right faces

    // Weighted sum
    float texSample = hatchXY * blend.z
                    + hatchXZ * blend.y
                    + hatchYZ * blend.x;

    // ── Lighting ─────────────────────────────────────────────────────────────
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

/*#version 330 core

in vec2 vUV;

out float fragHatch;

uniform sampler2D normalTex;
uniform sampler2D hatchTex;

uniform float uHatchScale;

uniform int   uIsInk;   

void main() {
    vec3 rawNormal = texture(normalTex, vUV).rgb;

    float grayDist = length(rawNormal - vec3(0.5));
    if (grayDist < 0.05) {
        fragHatch = 0.0;
        return;
    }

    vec3  lightDir = normalize(vec3(0.5, 1.0, 0.8));
    vec3  normal = normalize(rawNormal * 2.0 - 1.0);
    float NdotL = clamp(dot(normal, lightDir), 0.0, 1.0);
    float darkness = 1.0 - NdotL;

    float texSample = texture(hatchTex, vUV * uHatchScale).r;

    float stroke;
    if (uIsInk == 1) {
        // Ink wash: already dark where ink is heavy
        stroke = (1.0 - texSample) * darkness;
    } else {
        // Hatch lines: black lines on white → invert, then scale by darkness
        stroke = (1.0 - texSample) * darkness;
    }

    fragHatch = clamp(stroke, 0.0, 1.0);
}*/