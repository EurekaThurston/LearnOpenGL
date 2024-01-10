#version 330 core
out vec4 FragColor;

in vec3 nDir;
in vec3 posWS;
in vec2 UV;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct Light {
    vec4 lightVector;
    vec3 position;
    vec3 ambientInt;
    vec3 diffuseInt;
    vec3 specularInt;

    float Kc;
    float Kl;
    float Kq;
};

uniform Material material;
uniform Light light;
uniform vec3 baseCol;
uniform vec3 lightCol;
uniform vec3 camPos; 

// 世界空间光方向
vec3 WorldSpaceLightDir(vec3 posWS);
// 光强衰减
float LightAttenuation(vec3 posWS);


void main() {
    // 贴图载入
    vec3 var_diff = texture(material.diffuse, UV).rgb;
    vec3 var_spec = texture(material.specular, UV).rgb;
    // 环境光
    vec3 ambient = var_diff;

    // 漫反射
    vec3 nDirWS = normalize(nDir);
    vec3 lDirWS = WorldSpaceLightDir(posWS);
    float diff = max(dot(nDirWS, lDirWS), 0.0);
    vec3 diffuse =  diff * var_diff;

    // 镜面反射
    vec3 vDirWS = normalize(camPos - posWS);
    vec3 lrDirWS = reflect(-lDirWS, nDirWS);
    float spec = pow(max(dot(lrDirWS, vDirWS), 0.0), material.shininess);
    vec3 specular =  (spec * var_spec);

    // 最终输出
    ambient *= light.ambientInt;
    diffuse *= light.diffuseInt;
    specular *= light.specularInt;
    vec3 finalRGB = (ambient + diffuse + specular) * LightAttenuation(posWS);
    FragColor = vec4(finalRGB, 1.0);

}

vec3 WorldSpaceLightDir(vec3 posWS) {
    if (light.lightVector.w == 0.0) {
        return normalize(-light.lightVector.xyz);
    } else if (light.lightVector.w == 1.0) {
        return normalize(light.lightVector.xyz - posWS);
    }
}

float LightAttenuation(vec3 posWS) {
    float dist = length(light.position - posWS);
    return 1.0/(light.Kc + light.Kl*dist + light.Kq*(dist*dist));
}