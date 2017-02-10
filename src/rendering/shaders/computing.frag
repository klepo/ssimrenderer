#version 330

uniform samplerBuffer uT;
uniform samplerBuffer uPcs;

uniform int uWidth;
uniform int uHeight;

out float outColor;

noperspective in vec2 vPosition;

void main()
{
    int numberOfParameters = textureSize(uPcs);
    int index = int(floor(vPosition.y) * uWidth + floor(vPosition.x));

    float sum = 0;
    for (int i = 0; i < numberOfParameters; i++)
        sum += texelFetch(uT, index * numberOfParameters + i).r * texelFetch(uPcs, i).r;

    outColor = sum;
}
