#version 330

uniform sampler2D uOutputTexture;

noperspective in vec2 vTextureCoord;

out vec4 outColor;

void main()
{
    outColor = texture(uOutputTexture, vTextureCoord);
}
