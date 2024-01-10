#version 330 core
out vec4 FragColor;

in vec3 posWS; 
in vec3 nDirWS;
 

uniform vec3 camPosWS;
uniform samplerCube Cubemap;

void main() {
    float refractRatio = 1.0/1.309;
    vec3 nDirWS = normalize(nDirWS);
    vec3 vDirWS = normalize(posWS - camPosWS);       // 视方向的反方向    
    vec3 refractDirWS = refract(vDirWS, nDirWS, refractRatio);

    FragColor = vec4(texture(Cubemap, refractDirWS).rgb, 1.0);


}