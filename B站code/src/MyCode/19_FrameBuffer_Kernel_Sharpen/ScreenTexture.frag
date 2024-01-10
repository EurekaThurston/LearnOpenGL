#version 330 core
out vec4 FragColor;

in vec2 UV;

uniform sampler2D screenTexture;

const float offset = 1.0 / 450.0;

void main()
{ 
    // 每个核值所对应的像素距离中心像素的偏移量
    vec2 offsets[9] = vec2[] (
        vec2(-offset,  offset),      // 左上
        vec2(    0.0,  offset),      // 正上
        vec2( offset,  offset),      // 右上
        vec2(-offset,     0.0),      // 正左
        vec2(    0.0,     0.0),      // 正中
        vec2( offset,     0.0),      // 正右
        vec2(-offset, -offset),      // 左下
        vec2(    0.0, -offset),      // 正下
        vec2( offset, -offset)       // 右下
    );

    // 核矩阵(锐化)
    float kernel[9] = float[](
        -1, -1, -1,
        -1,  9, -1,
        -1, -1, -1
    );

    // 通过偏移量将中心像素以及周围的像素采样到sampleTex里
    vec3 sampleTex[9];
    for (int i = 0; i < 9; i++) {
        sampleTex[i] = vec3(texture(screenTexture, UV.st + offsets[i]));
    }

    // 中心像素最终的颜色
    vec3 col = vec3(0.0);
    for (int i = 0; i < 9; i++) {
        col += kernel[i] * sampleTex[i];
    }

    FragColor =vec4(col, 1.0);
}