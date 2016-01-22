#version 330

out vec4 outColor;
uniform vec4 uColor;

uniform sampler2D uTexture;

flat in int vTexture;
in vec2 vTextureCoord;

void main()
{
    outColor.a = gl_FragCoord.z;

    if (vTexture == 0) {
        outColor.rgb = uColor.rgb;
    } else {
        vec3 color = texture(uTexture, vTextureCoord).rgb;
        //float alpha = (color.r + color.g + color.b) / 3;
        //if (alpha == 0)
        //    discard;
        outColor.rgb = color.rgb;
    }
}
