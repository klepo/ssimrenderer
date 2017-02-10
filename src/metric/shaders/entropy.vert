#version 330

uniform sampler2D uHistogram0;
uniform sampler2D uHistogram1;
uniform sampler2D uJointHistogram;
//uniform int uRow;
uniform int uBins;

flat out vec4 vValue;

void main()
{
    vValue = vec4(0, 0, 0, 0);

    for (int i = 0; i < uBins; i++) {
        if (gl_VertexID == 0) {
            vec4 color0 = texelFetch(uHistogram0, ivec2(i, 0), 0);
            if (color0.r > 0)
                vValue.x += color0.r * log2(color0.r);

            vec4 color1 = texelFetch(uHistogram1, ivec2(i, 0), 0);
            if (color1.r > 0)
                vValue.y += color1.r * log2(color1.r);
        }

        vec4 color = texelFetch(uJointHistogram, ivec2(i, gl_VertexID), 0);
        if (color.r > 0)
            vValue.z += color.r * log2(color.r);
    }

    gl_Position = vec4(0, 0, 0, 1);
}
