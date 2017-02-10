#version 330

uniform sampler2D uInput;
uniform sampler2D uRenderingOutput;
uniform int uWidth;

flat out float vValue;

void main()
{
    float sum = 0;

    for (int i = 0; i < uWidth; i++) {
        vec4 value0 = texelFetch(uInput, ivec2(i, gl_VertexID), 0);
        vec4 value1 = texelFetch(uRenderingOutput, ivec2(i, gl_VertexID), 0);
        float diff = value0.r - value1.r;
        sum += diff * diff;
    }

    vValue = sum;
    gl_Position = vec4(0, 0, 0, 1);
}
