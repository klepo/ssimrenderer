#version 330

uniform int uWidth;
uniform int uHeight;

noperspective out vec2 vPosition;

void main()
{
    if (gl_VertexID == 0) {
        gl_Position = vec4(-1, -1, 0, 1);
        vPosition = ivec2(0, 0);
    }
    if (gl_VertexID == 1) {
        gl_Position = vec4(1, -1, 0, 1);
        vPosition = ivec2(uWidth, 0);
    }
    if (gl_VertexID == 2) {
        gl_Position = vec4(1, 1, 0, 1);
        vPosition = ivec2(uWidth, uHeight);
    }
    if (gl_VertexID == 3) {
        gl_Position = vec4(1, 1, 0, 1);
        vPosition = ivec2(uWidth, uHeight);
    }
    if (gl_VertexID == 4) {
        gl_Position = vec4(-1, 1, 0, 1);
        vPosition = ivec2(0, uHeight);
    }
    if (gl_VertexID == 5) {
        gl_Position = vec4(-1, -1, 0, 1);
        vPosition = ivec2(0, 0);
    }
}
