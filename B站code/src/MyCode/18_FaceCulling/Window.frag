#version 330 core
out vec4 FragColor;

in vec3 nDirWS;
in vec3 posWS;
in vec3 posVS;
in vec2 UV;

uniform sampler2D T_window;


void main() {
    vec4 var_window = texture(T_window, UV);

    FragColor = var_window;
}