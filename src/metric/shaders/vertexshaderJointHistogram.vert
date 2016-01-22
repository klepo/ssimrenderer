#version 330

//in ivec4 aPixel0;
//in ivec4 aPixel1;
uniform sampler2D uInput;
uniform sampler2D uRenderingOutput;
uniform int uRow;

void main()
{
    //float value0 = float(aPixel0.r) / 255.0f ;
    //float value1 = float(aPixel1.r) / 255.0f ;

    float value0 = texelFetch(uInput, ivec2(gl_VertexID, uRow), 0).r;
    float value1 = texelFetch(uRenderingOutput, ivec2(gl_VertexID, uRow), 0).r;

    float x = 2 * value0 - 1;
    if (x == 1)
        x = 0.999;
    if (x == -1)
        x = -0.999;

    float y = 2 * value1 - 1;
    if (y == 1)
        y = 0.999;
    if (y == -1)
        y = -0.999;

    gl_Position = vec4(x, y, 0, 1);
}
