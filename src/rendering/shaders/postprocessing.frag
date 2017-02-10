#version 330

uniform int uLineWidth;
uniform vec2 uTextureStep;

uniform sampler2D uDensityTexture;
uniform sampler2D uSilhouettesTexture;
uniform sampler2D uPolygonalTexture;
uniform sampler2D uPyramidTexture;
uniform bool uDensityEnabled;
uniform bool uSilhouettesEnabled;
uniform bool uPolygonalEnabled;
uniform bool uPyramidEnabled;
uniform float uIntensity;

noperspective in vec2 vTextureCoord;

layout(location = 0) out vec4 outColor;

void main()
{
    vec4 density = vec4(0, 0, 0, 0);
    vec4 polygonal = vec4(0, 0, 0, 0);
    vec4 pyramid = vec4(0, 0, 0, 0);

    if (uDensityEnabled)
        density = texture(uDensityTexture, vTextureCoord);

    if (uPolygonalEnabled)
        polygonal = texture(uPolygonalTexture, vTextureCoord);

    if (uPyramidEnabled)
        pyramid = texture(uPyramidTexture, vTextureCoord);

    float silhouetteMaxValue = 0;

    if (uSilhouettesEnabled) {
        int halfStepDown = int(floor(uLineWidth / 2.0f));
        int halfStepTop = int(ceil(uLineWidth / 2.0f));

        for (int x = -halfStepDown; x < halfStepTop; x++) {
            for (int y = -halfStepDown; y < halfStepTop; y++) {
                vec4 value = texture(uSilhouettesTexture, vec2(vTextureCoord.x + x * uTextureStep[0], vTextureCoord.y + y * uTextureStep[1]));
                silhouetteMaxValue = max(silhouetteMaxValue, value.a);
            }
        }
    }

    if (pyramid.a == 0 && density.r == 0 && polygonal.a == 0 && silhouetteMaxValue == 0) {
        discard;
    }

    if (pyramid.a != 0) {
        outColor = vec4(pyramid.rgb, 1.0f);
    }

    if (density.a != 0) {
        //density = 1 - exp(-density) * uIntensity;
        //float value = ((color.r - uMin) * 1.0) / (uMax - uMin);
        outColor = mix(outColor, vec4(1.0f, 1.0f, 1.0f, density.r * uIntensity), density.r * uIntensity);
    }

    if (polygonal.a != 0) {
        outColor = vec4(polygonal.rgb, 1.0f);
    }

    if (silhouetteMaxValue != 0) {
        outColor = vec4(1.0f, 0, 0, 1.0f);
    }
}
