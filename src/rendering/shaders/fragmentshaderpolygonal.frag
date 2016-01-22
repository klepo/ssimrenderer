#version 330

uniform bool uLightingEnabled;

out vec4 outColor;

in vec3 vColor;
smooth in vec3 vNormal;

vec3 lightColor = vec3(0.8f, 0.8f, 0.8f);
vec3 lightDirection = vec3(1.0f, 1.0f, -1.0f);
float lightAmbientIntensity = 0.7f;

void main()
{
    vec3 color = vColor;

    if (uLightingEnabled) {
        float diffuseIntensity = max(0.0f, dot(normalize(vNormal), lightDirection));
        //float cosTheta = clamp(dot(vNormal, lightDirection), 0, 1);
        color = vColor * vec3(lightColor * (lightAmbientIntensity + diffuseIntensity));
        //vec3 color = vColor * lightColor * cosTheta;
    }

    outColor = vec4(color, gl_FragCoord.z);
}
