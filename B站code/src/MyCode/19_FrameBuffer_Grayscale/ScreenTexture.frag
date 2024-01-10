#version 330 core
out vec4 FragColor;

in vec2 UV;

uniform sampler2D screenTexture;

void main()
{ 
    vec3 T_screenTexture = texture(screenTexture, UV).rgb;
    float colAverage = 0.2126 * T_screenTexture.r + 0.7152 * T_screenTexture.g + 0.0722 * T_screenTexture.b;
    FragColor =vec4(vec3(colAverage), 1.0);
}