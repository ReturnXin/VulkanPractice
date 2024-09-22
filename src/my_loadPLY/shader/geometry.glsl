#version 330 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

out vec3 outNormal;
out vec3 outFragPos;

in VS_OUT {
    vec3 outFragPos;
} gs_in[];

vec3 GetNormal() {
    vec3 a = vec3(gl_in[0].gl_Position - gl_in[1].gl_Position);
    vec3 b = vec3(gl_in[2].gl_Position - gl_in[1].gl_Position);
    return normalize(cross(a, b));
}

void main() {
    
    
    gl_Position = gl_in[0].gl_Position;
    outFragPos = gs_in[0].outFragPos;
    outNormal = GetNormal();
    EmitVertex();
    gl_Position = gl_in[1].gl_Position;
    outFragPos = gs_in[1].outFragPos;
    outNormal = GetNormal();
    EmitVertex();
    gl_Position = gl_in[2].gl_Position;
    outFragPos = gs_in[2].outFragPos;
    outNormal = GetNormal();
    EmitVertex();
    EndPrimitive();
}