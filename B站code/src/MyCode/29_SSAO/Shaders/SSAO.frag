#version 330 core
out float FragColor;

in vec2 UV;

uniform mat4 P;
uniform sampler2D gPositionDepth;
uniform sampler2D gNormal;
uniform sampler2D texNoise;
uniform vec3 samples[64];       // 采样点们

int kernelSize = 64;
float radius = 2.0;
float bias = 0.025;

const vec2 noiseScale = vec2(1920.0/4.0, 1080.0/4.0);


void main() {
    // 从G-buffer采图
    vec4 var_gPositionDepth = texture(gPositionDepth, UV);
    vec3 posVS = var_gPositionDepth.xyz;
    vec3 nDirVS = normalize(texture(gNormal, UV).rgb);
    vec3 randomVec = normalize(texture(texNoise, UV * noiseScale).xyz);

    // 创建切线空间->观察空间的TBN矩阵
    vec3 tDirVS = normalize(randomVec - nDirVS * dot(randomVec, nDirVS));
    vec3 bDirVS = normalize(cross(nDirVS, tDirVS));
    mat3 TBN = mat3(tDirVS, bDirVS, nDirVS);

    // 循环每个采样点并计算遮蔽率
    float occlusion = 0.0;
    for (int i = 0; i < kernelSize; ++i) {
        // 获取采样点在观察空间的位置
        vec3 samplePosVS = TBN * samples[i];            // 从切线空间变换到观察空间
        samplePosVS = posVS + samplePosVS * radius;     // 使当前采样点的原点移动到观察空间当前片段的位置上，并使用radius控制采样距离

        // 获取裁剪空间采样点位置
        vec4 offset = P * vec4(samplePosVS, 1.0);       // 观察空间->裁剪空间
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5 + 0.5;

        // 采样片段深度
        float sampleDepth = -texture(gPositionDepth, offset.xy).w;   // 获取到当前采样点所采样片段的深度
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(posVS.z - sampleDepth));
        occlusion += (sampleDepth >= samplePosVS.z + bias ? 1.0 : 0.0) * rangeCheck;
    }

    occlusion = 1.0 - (occlusion / kernelSize);

    FragColor = occlusion;
}