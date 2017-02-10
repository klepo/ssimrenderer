#version 330

noperspective out vec2 vTextureCoord;

void main()
{
    if (gl_VertexID == 0) {
        vTextureCoord = vec2(0, 0);
        gl_Position = vec4(-1, -1, 0, 1);
    }
    if (gl_VertexID == 1) {
        vTextureCoord = vec2(1, 0);
        gl_Position = vec4(1, -1, 0, 1);
    }
    if (gl_VertexID == 2) {
        vTextureCoord = vec2(1, 1);
        gl_Position = vec4(1, 1, 0, 1);
    }
    if (gl_VertexID == 3) {
        vTextureCoord = vec2(1, 1);
        gl_Position = vec4(1, 1, 0, 1);
    }
    if (gl_VertexID == 4) {
        vTextureCoord = vec2(0, 1);
        gl_Position = vec4(-1, 1, 0, 1);
    }
    if (gl_VertexID == 5) {
        vTextureCoord = vec2(0, 0);
        gl_Position = vec4(-1, -1, 0, 1);
    }
}
