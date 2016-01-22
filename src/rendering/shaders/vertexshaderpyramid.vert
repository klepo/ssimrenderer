#version 330

uniform mat4 uMatrix;
uniform mat4 uCorners;
uniform vec3 uEye;
out vec2 vTextureCoord;
flat out int vTexture;

void main()
{
    vTexture = 0;
    if (gl_VertexID == 0) {
        gl_Position = uMatrix * vec4(uEye, 1.0f);
    }
    if (gl_VertexID == 1) {
        gl_Position = uMatrix * uCorners[0];
    }
    if (gl_VertexID == 2) {
        gl_Position = uMatrix * uCorners[1];
    }

    if (gl_VertexID == 3) {
        gl_Position = uMatrix * vec4(uEye, 1.0f);
    }
    if (gl_VertexID == 4) {
        gl_Position = uMatrix * uCorners[3];
    }
    if (gl_VertexID == 5) {
        gl_Position = uMatrix * uCorners[2];
    }

    if (gl_VertexID == 6) {
        gl_Position = uMatrix * vec4(uEye, 1.0f);
    }
    if (gl_VertexID == 7) {
        gl_Position = uMatrix * uCorners[2];
    }
    if (gl_VertexID == 8) {
        gl_Position = uMatrix * uCorners[0];
    }

    if (gl_VertexID == 9) {
        gl_Position = uMatrix * vec4(uEye, 1.0f);
    }
    if (gl_VertexID == 10) {
        gl_Position = uMatrix * uCorners[1];
    }
    if (gl_VertexID == 11) {
        gl_Position = uMatrix * uCorners[3];
    }

    if (gl_VertexID == 12) {
        gl_Position = uMatrix * uCorners[0];
        vTexture = 1;
        vTextureCoord = vec2(1, 1);
    }
    if (gl_VertexID == 13) {
        gl_Position = uMatrix * uCorners[1];
        vTexture = 1;
        vTextureCoord = vec2(1, 0);
    }
    if (gl_VertexID == 14) {
        gl_Position = uMatrix * uCorners[2];
        vTexture = 1;
        vTextureCoord = vec2(0, 1);
    }

    if (gl_VertexID == 15) {
        gl_Position = uMatrix * uCorners[2];
        vTexture = 1;
        vTextureCoord = vec2(0, 1);
    }
    if (gl_VertexID == 16) {
        gl_Position = uMatrix * uCorners[3];
        vTexture = 1;
        vTextureCoord = vec2(0, 0);
    }
    if (gl_VertexID == 17) {
        gl_Position = uMatrix * uCorners[1];
        vTexture = 1;
        vTextureCoord = vec2(1, 0);
    }


}
