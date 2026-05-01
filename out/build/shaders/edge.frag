#version 330 core

in vec2 vUV;
out float fragEdge;

uniform sampler2D normalTex;
uniform float uEdgeThreshold;

bool isBackground(vec3 raw) {
    return (abs(raw.r - 0.5) < 0.04 &&
            abs(raw.g - 0.5) < 0.04 &&
            abs(raw.b - 0.5) < 0.04);
}

vec3 sampleNormal(vec2 uv, vec2 offset) {
    vec2 texelSize = 1.0 / vec2(textureSize(normalTex, 0));
    vec3 raw = texture(normalTex, uv + offset * texelSize).rgb;
    return isBackground(raw) ? vec3(0.0, 0.0, 1.0) : raw * 2.0 - 1.0;
}

void main() {
    vec3 centerRaw = texture(normalTex, vUV).rgb;
    if (isBackground(centerRaw)) { fragEdge = 0.0; return; }

    vec3 n00 = sampleNormal(vUV, vec2(-1.0,-1.0));
    vec3 n10 = sampleNormal(vUV, vec2( 0.0,-1.0));
    vec3 n20 = sampleNormal(vUV, vec2( 1.0,-1.0));
    vec3 n01 = sampleNormal(vUV, vec2(-1.0, 0.0));
    vec3 n21 = sampleNormal(vUV, vec2( 1.0, 0.0));
    vec3 n02 = sampleNormal(vUV, vec2(-1.0, 1.0));
    vec3 n12 = sampleNormal(vUV, vec2( 0.0, 1.0));
    vec3 n22 = sampleNormal(vUV, vec2( 1.0, 1.0));

    vec3 gx = -n00 - 2.0*n01 - n02 + n20 + 2.0*n21 + n22;
    vec3 gy = -n00 - 2.0*n10 - n20 + n02 + 2.0*n12 + n22;

    float edge = length(gx) + length(gy);
    fragEdge = smoothstep(uEdgeThreshold, uEdgeThreshold + 0.5, edge);
}