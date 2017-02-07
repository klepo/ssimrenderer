#version 330

uniform sampler2D uPositionDiff;
uniform bool uXMirror;

//in ivec3 aIndicesX;
//in ivec3 aIndicesY;
in vec3 aPosition;
in vec3 aColor;
in vec3 aNormal;

out vec3 vColor;
smooth out vec3 vNormal;

uniform mat4 uMatrix;
uniform mat3 uNormalMatrix;

uniform int uPositionDiffLengthMinus1;
uniform int uPositionDiffLengthLog2;

void main()
{
    vec3 position = aPosition;
    vec3 positionDiff;
    vec3 normal = aNormal;

    positionDiff.x = texelFetch(uPositionDiff, ivec2((gl_VertexID * 3) & uPositionDiffLengthMinus1, (gl_VertexID * 3) >> uPositionDiffLengthLog2), 0).r;
    positionDiff.y = texelFetch(uPositionDiff, ivec2((gl_VertexID * 3 + 1) & uPositionDiffLengthMinus1, (gl_VertexID * 3 + 1) >> uPositionDiffLengthLog2), 0).r;
    positionDiff.z = texelFetch(uPositionDiff, ivec2((gl_VertexID * 3 + 2) & uPositionDiffLengthMinus1, (gl_VertexID * 3 + 2) >> uPositionDiffLengthLog2), 0).r;
    //positionDiff.x = texelFetch(uPositionDiff, ivec2(aIndicesX.x, aIndicesY.x), 0).r;
    //positionDiff.y = texelFetch(uPositionDiff, ivec2(aIndicesX.y, aIndicesY.y), 0).r;
    //positionDiff.z = texelFetch(uPositionDiff, ivec2(aIndicesX.z, aIndicesY.z), 0).r;

    if (uXMirror) {
        position.x = -position.x;
        positionDiff.x = -positionDiff.x;
        normal.x = -normal.x;
    }

    gl_Position = uMatrix * vec4(position + positionDiff, 1.0f);
    vColor = aColor;
    if (vColor == vec3(0.0, 0.0, 0.0))
        vColor = vec3(0.5, 0.5, 0.5);
    vNormal = uNormalMatrix * normal;
}
