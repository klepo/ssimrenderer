#version 330

uniform sampler2D uJointHistogram;
uniform bool uFlag;
uniform int uRow;
uniform int uBins;

flat out float vValue;

void main()
{
    vec4 color;

    if (uFlag)
        color = texelFetch(uJointHistogram, ivec2(gl_VertexID, uRow), 0);
    else
        color = texelFetch(uJointHistogram, ivec2(uRow, gl_VertexID), 0);

    vValue = color.r;

    float x = 2 * float(uRow) / (float(uBins) - 1) - 1;
    if (x == 1)
        x = 0.999;
    if (x == -1)
        x = -0.999;

    gl_Position = vec4(x, 0, 0, 1);
}
