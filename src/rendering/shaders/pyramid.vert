#version 330

uniform mat4 uMatrix;
uniform mat4 uCorners;
uniform vec3 uEye;
out vec2 vTextureCoord;
flat out int vTexture;

void main()
{
    if (gl_VertexID == 0) {
        vTexture = 0;
        gl_Position = uMatrix * vec4(uEye, 1.0f);
    }
    if (gl_VertexID == 1) {
        vTexture = 0;
        gl_Position = uMatrix * uCorners[0];
    }
    if (gl_VertexID == 2) {
        vTexture = 0;
        gl_Position = uMatrix * uCorners[1];
    }

    if (gl_VertexID == 3) {
        vTexture = 0;
        gl_Position = uMatrix * vec4(uEye, 1.0f);
    }
    if (gl_VertexID == 4) {
        vTexture = 0;
        gl_Position = uMatrix * uCorners[3];
    }
    if (gl_VertexID == 5) {
        vTexture = 0;
        gl_Position = uMatrix * uCorners[2];
    }

    if (gl_VertexID == 6) {
        vTexture = 0;
        gl_Position = uMatrix * vec4(uEye, 1.0f);
    }
    if (gl_VertexID == 7) {
        vTexture = 0;
        gl_Position = uMatrix * uCorners[2];
    }
    if (gl_VertexID == 8) {
        vTexture = 0;
        gl_Position = uMatrix * uCorners[0];
    }

    if (gl_VertexID == 9) {
        vTexture = 0;
        gl_Position = uMatrix * vec4(uEye, 1.0f);
    }
    if (gl_VertexID == 10) {
        vTexture = 0;
        gl_Position = uMatrix * uCorners[1];
    }
    if (gl_VertexID == 11) {
        vTexture = 0;
        gl_Position = uMatrix * uCorners[3];
    }

    if (gl_VertexID == 12) {
        gl_Position = uMatrix * uCorners[0];
        vTexture = 1;
        vTextureCoord = vec2(0, 0);
    }
    if (gl_VertexID == 13) {
        gl_Position = uMatrix * uCorners[1];
        vTexture = 1;
        vTextureCoord = vec2(0, 1);
    }
    if (gl_VertexID == 14) {
        gl_Position = uMatrix * uCorners[2];
        vTexture = 1;
        vTextureCoord = vec2(1, 0);
    }

    if (gl_VertexID == 15) {
        gl_Position = uMatrix * uCorners[2];
        vTexture = 1;
        vTextureCoord = vec2(1, 0);
    }
    if (gl_VertexID == 16) {
        gl_Position = uMatrix * uCorners[1];
        vTexture = 1;
        vTextureCoord = vec2(0, 1);
    }
    if (gl_VertexID == 17) {
        gl_Position = uMatrix * uCorners[3];
        vTexture = 1;
        vTextureCoord = vec2(1, 1);
    }


}
