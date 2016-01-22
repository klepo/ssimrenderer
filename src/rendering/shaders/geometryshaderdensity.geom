#version 330

layout (lines_adjacency) in;
layout (triangle_strip, max_vertices = 12) out;

uniform mat4 uMatrix;
uniform mat4 uMatrixInv;

uniform bool uXMirror;

out vec4 b;
out vec4 bEyedir;
out vec4 eEyedir;

mat4 e;
mat4 eEye;
mat4 bAll;
mat4 bEyedirAll;

void emitVertex(int i)
{
    gl_Position = e[i];
    b = bAll[i];
    if (uXMirror) {
        bEyedir = -bEyedirAll[i];
        eEyedir = -eEye[i] - e[i];
    } else {
        bEyedir = bEyedirAll[i];
        eEyedir = eEye[i] - e[i];
    }
    EmitVertex();
}

void emitPrimitive(int i, int j, int k)
{
    gl_PrimitiveID = gl_PrimitiveIDIn;
    emitVertex(i);
    emitVertex(j);
    emitVertex(k);
    EndPrimitive();
}

void main()
{
    mat4 w = mat4(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_in[2].gl_Position, gl_in[3].gl_Position);
    e = uMatrix * w;
    eEye = e;
    eEye[0] = eEye[0] / eEye[0].w;
    eEye[1] = eEye[1] / eEye[1].w;
    eEye[2] = eEye[2] / eEye[2].w;
    eEye[3] = eEye[3] / eEye[3].w;
    eEye[0].z = 0;
    eEye[1].z = 0;
    eEye[2].z = 0;
    eEye[3].z = 0;
    bAll = mat4(
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        );
    bEyedirAll = bAll - (mat4(inverse(w) * uMatrixInv * eEye));

    emitPrimitive(0, 2, 1);
    emitPrimitive(1, 2, 3);
    emitPrimitive(3, 2, 0);
    emitPrimitive(0, 1, 3);
}
