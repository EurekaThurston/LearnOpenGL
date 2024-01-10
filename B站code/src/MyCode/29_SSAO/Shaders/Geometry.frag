#version 330 core 
layout (location = 0) out vec4 gPositionDepth;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gColorSpec;

in VS_OUT {
    vec3 posVS; 
    vec3 nDirVS;
    vec2 UV;
} FragInput;

uniform sampler2D Diffuse;
uniform sampler2D Specular;
uniform vec3 Color;

const float NEAR = 0.1;
const float FAR = 100.0;

float LinearSceneDepth(float depth) {
    float z = depth * 2.0 - 1.0;
    return  (2.0 * NEAR * FAR) / (FAR + NEAR - z * (FAR - NEAR));
}

void main() {
    gPositionDepth.rgb = FragInput.posVS;
    gPositionDepth.a = LinearSceneDepth(gl_FragCoord.z);
    gNormal = normalize(FragInput.nDirVS);
    gColorSpec.rgb = Color;
    gColorSpec.a = 0.0; 

}