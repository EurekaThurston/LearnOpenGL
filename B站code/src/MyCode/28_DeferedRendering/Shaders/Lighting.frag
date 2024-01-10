#version 330 core 
out vec4 FragColor;
in vec2 UV;


struct Light {
    vec3 Position; 
    vec3 Color;
};

const int LightNum = 32;
uniform Light lights[LightNum];
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gColorSpec;
uniform vec3 camPos;

void main() {
    const float gamma = 2.2;
    // 从G-buffer采样获取场景数据
    vec4 var_gColorSpec = texture(gColorSpec, UV); 
    vec3 posWS = texture(gPosition, UV).rgb;
    vec3 nDirWS = texture(gNormal, UV).rgb;
    vec3 diffCol = var_gColorSpec.rgb;
    float specInt = var_gColorSpec.a;

    // 向量准备
    vec3 vDirWS = normalize(camPos - posWS);

    // -----光照计算-----
    // Ambient
    vec3 ambient = diffCol * 0.1;

    // 最终结果
    vec3 finalRGB = ambient;

    // 逐光源计算光照信息
    for (int i = 0; i < LightNum; i++) {
        // 向量准备
        vec3 lDirWS = normalize(lights[i].Position - posWS);
        vec3 hDirWS = normalize(lDirWS + vDirWS);
        // 漫反射
        float nDotL = max(0.0, dot(nDirWS, lDirWS));
        vec3 diffuse = nDotL * diffCol * lights[i].Color;

        // 镜面反射
        float nDotH = pow(max(0.0, dot(nDirWS, hDirWS)), 16.0);
        vec3 specular = nDotH * specInt * lights[i].Color;

        // 光照衰减
        float dist = length(lights[i].Position - posWS);
        float attenuation = 1.0/(dist * dist);

        finalRGB += (diffuse + specular) * attenuation;
    }
    finalRGB = pow(finalRGB, vec3(1.0/gamma));

    FragColor = vec4(finalRGB, 1.0);



}