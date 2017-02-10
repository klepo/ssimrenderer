#version 330

out vec4 outColor;

flat in vec4 vValue;

void main()
{
    outColor = vValue;
}
