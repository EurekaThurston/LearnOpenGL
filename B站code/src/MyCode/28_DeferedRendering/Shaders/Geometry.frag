#version 330 core 
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gColorSpec;

in VS_OUT {
    vec3 posWS; 
    vec3 nDirWS;
    vec2 UV;
} FragInput;

uniform sampler2D Diffuse;
uniform sampler2D Specular;

void main() {
    vec3 T_Diffuse = texture(Diffuse, FragInput.UV).rgb;
    vec3 T_Specular = texture(Specular, FragInput.UV).rgb;
    gPosition = FragInput.posWS;
    gNormal = normalize(FragInput.nDirWS);
    gColorSpec.rgb = T_Diffuse;
    gColorSpec.a = T_Specular.r; 

}