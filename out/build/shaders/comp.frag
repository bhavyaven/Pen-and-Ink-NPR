#version 330 core

in vec2 vUV;
out vec4 fragColor;

uniform sampler2D edgeTex;
uniform sampler2D hatchTex;
uniform sampler2D paperTex;

uniform float uEdgeBoost;
uniform float uHatchAlpha;
uniform float uPaperScale;

uniform int   uDebug;

const vec3 INK_COLOR = vec3(0.08, 0.05, 0.02);

void main() {
    float hatchMask = texture(hatchTex, vUV).r;

    if (uDebug == 1) {
        fragColor = vec4(vec3(hatchMask), 1.0);
        return;
    }

    vec3  paper = texture(paperTex, vUV * uPaperScale).rgb;
    float edge = clamp(texture(edgeTex, vUV).r * uEdgeBoost, 0.0, 1.0);
    float hatchFinal = clamp(hatchMask * uHatchAlpha, 0.0, 1.0);
    float ink = clamp(edge + hatchFinal * (1.0 - edge), 0.0, 1.0);

    fragColor = vec4(mix(paper, INK_COLOR, ink), 1.0);
}