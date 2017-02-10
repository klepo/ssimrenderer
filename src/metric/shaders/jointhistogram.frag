#version 330

out float outColor;

uniform float uNormalizedOutputUnit;

void main()
{
    outColor = uNormalizedOutputUnit;
}
