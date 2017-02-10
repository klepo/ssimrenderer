#version 330

//uniform float uLineWidth;

//flat in vec4 vCenter;
//flat in int vConnections;
in vec3 vvPosition;
//in vec4 vCenterLine;

out vec4 outColor;

void main()
{
    //if (vPosition.z < 100)
        //discard;
    /*if (vConnections == 1) {
        float d = distance(vCenter.xy, vPosition.xy);
        if (d < uLineWidth) {
            //discard;
            outColor = vec4(vCenter.xyz, gl_FragCoord.z / gl_FragCoord.w);
            //outColor.r = 0.8f;
        } else {
            discard;
        }
    } else {
        //discard;
        outColor = vec4(vCenter.xyz, gl_FragCoord.z / gl_FragCoord.w);
        //outColor.r = 0.8f;
    }*/

    outColor = vec4(vvPosition, gl_FragCoord.z);
}
