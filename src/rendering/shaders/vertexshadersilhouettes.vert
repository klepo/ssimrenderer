#version 330

uniform sampler2D uPositionDiff;
uniform bool uXMirror;

//in ivec3 aIndicesX;
//in ivec3 aIndicesY;
in vec3 aPosition;
out vec3 vPosition;

uniform mat4 uMatrix;

uniform int uPositionDiffLengthMinus1;
uniform int uPositionDiffLengthLog2;

void main()
{
    vec3 position = aPosition;
    vec3 positionDiff;

    positionDiff.x = texelFetch(uPositionDiff, ivec2((gl_VertexID * 3) & uPositionDiffLengthMinus1, (gl_VertexID * 3) >> uPositionDiffLengthLog2), 0).r;
    positionDiff.y = texelFetch(uPositionDiff, ivec2((gl_VertexID * 3 + 1) & uPositionDiffLengthMinus1, (gl_VertexID * 3 + 1) >> uPositionDiffLengthLog2), 0).r;
    positionDiff.z = texelFetch(uPositionDiff, ivec2((gl_VertexID * 3 + 2) & uPositionDiffLengthMinus1, (gl_VertexID * 3 + 2) >> uPositionDiffLengthLog2), 0).r;
    //positionDiff.x = texelFetch(uPositionDiff, ivec2(aIndicesX.x, aIndicesY.x), 0).r;
    //positionDiff.y = texelFetch(uPositionDiff, ivec2(aIndicesX.y, aIndicesY.y), 0).r;
    //positionDiff.z = texelFetch(uPositionDiff, ivec2(aIndicesX.z, aIndicesY.z), 0).r;

    if (uXMirror) {
        position.x = -position.x;
        positionDiff.x = -positionDiff.x;
    }

    gl_Position = uMatrix * vec4(position + positionDiff, 1.0f);
    vPosition = vec3(aPosition + positionDiff);
}
