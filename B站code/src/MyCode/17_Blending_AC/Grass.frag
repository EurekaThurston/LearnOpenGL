#version 330 core
out vec4 FragColor;

in vec3 nDirWS;
in vec3 posWS;
in vec3 posVS;
in vec2 UV;

uniform sampler2D T_Grass;


void main() {
    vec4 var_Grass = texture(T_Grass, UV);
    if (var_Grass.a < 0.1) {
        discard;
    }

    FragColor = var_Grass;
}