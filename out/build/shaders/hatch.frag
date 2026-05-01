#version 330 core
in vec2 vUV;
out float fragHatch;
uniform sampler2D normalTex;
uniform sampler2D hatchTex;
uniform float uHatchScale;
uniform int   uIsInk;   // 1 = ink wash texture (dark on light), 0 = hatch (light on dark)

void main() {
    vec3 rawNormal = texture(normalTex, vUV).rgb;

    float grayDist = length(rawNormal - vec3(0.5));
    if (grayDist < 0.05) {
        fragHatch = 0.0;
        return;
    }

    vec3  lightDir = normalize(vec3(0.5, 1.0, 0.8));
    vec3  normal   = normalize(rawNormal * 2.0 - 1.0);
    float NdotL    = clamp(dot(normal, lightDir), 0.0, 1.0);
    float darkness = 1.0 - NdotL;

    float texSample = texture(hatchTex, vUV * uHatchScale).r;

    float stroke;
    if (uIsInk == 1) {
        // Ink wash: already dark where ink is heavy (low r = lots of ink)
        // Don't invert — dark pixels ARE the ink
        // Use darkness to mask: only show ink in shadowed regions
        stroke = (1.0 - texSample) * darkness;
    } else {
        // Hatch lines: black lines on white → invert, then scale by darkness
        stroke = (1.0 - texSample) * darkness;
    }

    fragHatch = clamp(stroke, 0.0, 1.0);
}