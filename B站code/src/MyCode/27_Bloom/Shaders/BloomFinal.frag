#version 330 core 
out vec4 FragColor;

in vec2 UV;

uniform sampler2D HDRScene;
uniform sampler2D BlurScene;
uniform float exposure;

void main() {
    const float gamma = 2.2;
    vec3 T_HDRScene = texture(HDRScene, UV).rgb;
    vec3 T_BlurScene = texture(BlurScene, UV).rgb;

    vec3 HDRScene = T_HDRScene + T_BlurScene;
    vec3 finalRGB = vec3(1.0) - exp(-HDRScene * exposure);
    finalRGB = pow(finalRGB, vec3(1.0/gamma));

    FragColor = vec4(finalRGB, 1.0);
}