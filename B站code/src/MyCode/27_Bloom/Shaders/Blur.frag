#version 330 core
out vec4 FragColor;

in vec2 UV;

uniform sampler2D OriginalScene;
uniform bool horizontal;
uniform float weight[10] = float[] (0.1974129434306977, 0.17466680058027237, 0.12097768865668214, 0.06559086081218723, 0.027834737777226178, 0.009244634006751011, 0.002402737087917116, 0.0004886429196522016, 0.00007774906664655715, 0.000009677377316317854);    // 当前片段的一侧的片段的权重值

void main() {
    vec2 offsetUnit = 1.0 / textureSize(OriginalScene, 0);      // 获取到一个纹素的大小并计算出单位偏移值
    vec3 result = texture(OriginalScene, UV).rgb * weight[0];   // 当前片段的权重值

    if (horizontal) {
        for (int i = 1; i < 10; ++i) {
            result += texture(OriginalScene, UV + vec2(offsetUnit.x * i, 0.0)).rgb * weight[i];    // 当前片段右侧的第i个片段的权重
            result += texture(OriginalScene, UV - vec2(offsetUnit.x * i, 0.0)).rgb * weight[i];    // 当前片段左侧的第i个片段的权重
        }
    } else {
        for (int i = 1; i < 10; ++i) {
            result += texture(OriginalScene, UV + vec2(0.0, offsetUnit.y * i)).rgb * weight[i];    // 当前片段上方的第i个片段的权重
            result += texture(OriginalScene, UV - vec2(0.0, offsetUnit.y * i)).rgb * weight[i];    // 当前片段下方的第i个片段的权重
        }
    }

    FragColor = vec4(result, 1.0);
}