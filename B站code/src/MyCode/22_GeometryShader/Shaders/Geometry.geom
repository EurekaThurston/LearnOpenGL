#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT {
    vec2 texCoords;
    vec3 normal;
} GeometryInput[];

out vec2 UV;
out vec3 nDirWS;

uniform float time;

vec3 GetNormal() {
    vec3 a = vec3(gl_in[1].gl_Position) - vec3(gl_in[0].gl_Position);
    vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[0].gl_Position);
    return normalize(cross(b, a));
}

vec4 Explode(vec4 position, vec3 normal) {
    float intensity = 2.0;
    vec3 direction = normal * intensity * ((sin(time) + 1.0)/2.0);
    return position + vec4(direction, 0.0);
}

void main() {    
    vec3 nDirWS = GetNormal();

    gl_Position = Explode(gl_in[0].gl_Position, nDirWS);
    UV = GeometryInput[0].texCoords;
    EmitVertex();
    
    gl_Position = Explode(gl_in[1].gl_Position, nDirWS);
    UV = GeometryInput[1].texCoords;
    EmitVertex();

    gl_Position = 
    Explode(gl_in[2].gl_Position, nDirWS);
    UV = GeometryInput[2].texCoords;
    EmitVertex();

    EndPrimitive();

}