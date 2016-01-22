#version 330

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

smooth in vec3 vColor[];
smooth out vec3 vvColor;

in vec3 vNormal[];
out vec3 vvNormal;

void main()
{
    gl_Position = gl_in[0].gl_Position;
    vvColor = vColor[0];
    vvNormal = vNormal[0];
    EmitVertex();
    gl_Position = gl_in[2].gl_Position;
    vvColor = vColor[2];
    vvNormal = vNormal[2];
    EmitVertex();
    gl_Position = gl_in[1].gl_Position;
    vvColor = vColor[1];
    vvNormal = vNormal[1];
    EmitVertex();
    EndPrimitive();
}
