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

// 平行光数据结构
struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float intensity;
};

// 点光源数据结构
struct PointLight {
    vec3 position;
    float Kc;
    float Kl;
    float Kq;

    float intensity;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float innerCutOffAngleCos;
    float outerCutOffAngleCos;
    float Kc;
    float Kl;
    float Kq;

    float intensity;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float lightSwitch;

    sampler2D lightTexture;
};

#define NR_POINT_LIGHTS 4       // 定义场景中点光源的数量

uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform Material material;
uniform vec3 camPos; 


vec3 CalcDirLight(DirLight light, vec3 nDirWS, vec3 vDirWS);
vec3 CalcPointLight(PointLight light, vec3 nDirWS, vec3 posWS, vec3 vDirWS);
vec3 CalcSpotLight(SpotLight light, vec3 nDirWS, vec3 posWS, vec3 vDirWS);

void main() {
    // 贴图采样
    vec3 T_diffuse = texture(material.diffuse, UV).rgb;
    vec3 T_specular = texture(material.specular, UV).rgb;

    // 向量准备
    vec3 nDirWS = normalize(nDir);
    vec3 vDirWS = normalize(camPos - posWS);

    // 平行光
    vec3 dirLight = CalcDirLight(dirLight, nDirWS, vDirWS);

    // 点光源
    vec3 pointLight;
    for (int i = 0; i < NR_POINT_LIGHTS; i++) {
        pointLight += CalcPointLight(pointLights[i], nDirWS, posWS, vDirWS);
    }

    // 聚光灯
    vec3 spotLight = CalcSpotLight(spotLight, nDirWS, posWS, vDirWS);

    // 颜色输出
    vec3 finalRGB = (dirLight + pointLight + spotLight);
    FragColor = vec4(finalRGB, 1.0);
}


vec3 CalcDirLight(DirLight light, vec3 nDirWS, vec3 vDirWS) {
    // 贴图采样
    vec3 T_diffuse = texture(material.diffuse, UV).rgb;
    vec3 T_specular = texture(material.specular, UV).rgb;

    // 向量准备
    vec3 lDirWS = normalize(-light.direction);
    vec3 lrDirWS = reflect(-lDirWS, nDirWS);

    // 漫反射
    float diff = max(dot(nDirWS, lDirWS), 0.0);
    vec3 diffuse = diff * T_diffuse * light.diffuse;

    // 镜面反射
    float spec = pow(max(dot(lrDirWS, vDirWS), 0.0), material.shininess);
    vec3 specular = spec * T_specular * light.specular;

    // 环境光照
    vec3 ambient = T_diffuse * light.ambient;

    return (ambient + diffuse + specular) * light.intensity;
}

vec3 CalcPointLight(PointLight light, vec3 nDirWS, vec3 posWS, vec3 vDirWS) {
    // 贴图采样
    vec3 T_diffuse = texture(material.diffuse, UV).rgb;
    vec3 T_specular = texture(material.specular, UV).rgb;

    // 向量准备
    vec3 lDirWS = normalize(light.position - posWS);
    vec3 lrDirWS = reflect(-lDirWS, nDirWS);

    // 光强衰减
    float dist = length(light.position - posWS);
    float attenuation = 1.0/(light.Kc + light.Kl*dist + light.Kq*dist*dist);

    // 漫反射
    float diff = max(dot(nDirWS, lDirWS), 0.0);
    vec3 diffuse = diff * light.diffuse * attenuation * light.intensity * T_diffuse;

    // 镜面反射
    float spec = pow(max(dot(vDirWS, lrDirWS), 0.0), material.shininess);
    vec3 specular = spec * light.specular * attenuation * light.intensity * T_specular;

    // 环境光照
    vec3 ambient = light.ambient * attenuation * light.intensity * T_diffuse;

    return  (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 nDirWS, vec3 posWS, vec3 vDirWS) {
    if (light.lightSwitch == 1.0) {
        // 贴图采样
        vec3 T_diffuse = texture(material.diffuse, UV).rgb;
        vec3 T_specular = texture(material.specular, UV).rgb;
        vec3 T_azi = texture(light.lightTexture, UV).rgb;

        // 向量准备
        vec3 lDirWS = normalize(light.position - posWS);
        vec3 lrDirWS = reflect(-lDirWS, nDirWS);   

        // 聚光灯光照范围
        float theta = dot(normalize(-light.direction), vDirWS);
        float epsilon = light.innerCutOffAngleCos - light.outerCutOffAngleCos;
        float intensity = clamp((theta - light.outerCutOffAngleCos)/epsilon, 0.0, 1.0);

        // 光强衰减
        float dist = length(light.position - posWS);
        float attenuation = 1.0/(light.Kc + light.Kl*dist + light.Kq*dist*dist);

        // 漫反射
        float diff = max(dot(nDirWS, lDirWS), 0.0);
        vec3 diffuse = diff * T_diffuse * light.diffuse * attenuation; 

        // 镜面反射
        float spec = pow(max(dot(lrDirWS, vDirWS), 0.0), material.shininess);
        vec3 specular = spec * T_specular * light.specular * attenuation;

        // 环境光照
        vec3 ambient = T_diffuse * light.ambient * attenuation;

        return (ambient + diffuse + specular) * light.intensity  * intensity;
    } else {
        return vec3(0.0);
    }
    
}