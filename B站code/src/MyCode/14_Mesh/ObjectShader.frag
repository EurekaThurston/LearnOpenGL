#version 330 core
out vec4 FragColor;

in vec3 nDirWS;
in vec3 posWS;
in vec2 UV;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform vec3 lDir; 
uniform vec3 camPos;

void main() {
    // 贴图采样
    vec3 T_diffuse = texture(texture_diffuse1, UV).rgb;
    vec3 T_specular = texture(texture_specular1, UV).rgb;

    // 向量准备
    vec3 lDirWS = normalize(lDir);
    vec3 vDirWS = normalize(camPos - posWS);
    vec3 lrDirWS = reflect(-lDirWS, nDirWS);

    // 向量计算
    float NdotL = max(dot(nDirWS, lDirWS), 0.0);
    float VdotLr = max(dot(vDirWS, lrDirWS), 0.0);

    // 漫反射
    vec3 diffuse = NdotL * T_diffuse;

    // 镜面反射
    vec3 specular = pow(VdotLr, 32.0) * T_specular;

    vec3 finalRGB = (diffuse + specular) * 2.5;

    FragColor = vec4(finalRGB, 1.0);
}