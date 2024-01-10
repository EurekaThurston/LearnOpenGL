#version 330 core 
out vec4 FragColor;
in vec2 UV;

uniform vec3 lightPos;
uniform vec3 lightCol;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gColorSpec;
uniform sampler2D SSAO;
uniform vec3 camPos;
uniform mat4 P;

void main() {
    const float gamma = 2.2;
    // 从G-buffer采样获取场景数据
    vec4 var_gColorSpec = texture(gColorSpec, UV); 
    vec3 posVS = texture(gPosition, UV).rgb;
    vec3 nDirVS = texture(gNormal, UV).rgb;
    vec3 diffCol = var_gColorSpec.rgb;
    float AO = texture(SSAO, UV).r;
    AO *= AO;
    float specInt = var_gColorSpec.a;

    // 向量准备
    vec3 vDirVS = normalize(-posVS);

    // -----光照计算-----
    // Ambient
    vec3 ambient = diffCol * 0.1 * AO;

    // 逐光源计算光照信息
    // 向量准备
    vec3 lDirVS = normalize(lightPos - posVS);
    vec3 hDirVS = normalize(lDirVS + vDirVS);
    // 漫反射
    float nDotL = max(0.0, dot(nDirVS, lDirVS));
    vec3 diffuse = nDotL * diffCol * lightCol;

    // 镜面反射
    float nDotH = pow(max(0.0, dot(nDirVS, hDirVS)), 16.0);
    vec3 specular = nDotH * lightCol;

    // 光照衰减
    float dist = length(lightPos - posVS);
    float attenuation = 1.0/(dist * dist);

    vec3 finalRGB = (diffuse + specular) * attenuation + ambient;

    finalRGB = pow(finalRGB, vec3(1.0/gamma));

    // FragColor = vec4(AO, 0.0, 0.0, 1.0);
    FragColor = vec4(finalRGB, 1.0);



}