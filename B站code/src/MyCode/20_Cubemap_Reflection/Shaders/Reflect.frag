#version 330 core
out vec4 FragColor;

in vec3 posWS; 
in vec3 nDirWS;
 

uniform vec3 camPosWS;
uniform samplerCube Cubemap;

void main() {
    vec3 nDirWS = normalize(nDirWS);
    vec3 vDirWS = normalize(posWS - camPosWS);       // 视方向的反方向    
    vec3 reflectDirWS = reflect(vDirWS, nDirWS);

    FragColor = vec4(texture(Cubemap, reflectDirWS).rgb, 1.0);


}