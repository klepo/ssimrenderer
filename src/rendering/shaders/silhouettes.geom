#version 330

layout (triangles_adjacency) in;
layout (line_strip, max_vertices = 6) out;

//uniform float uLineWidth;
//uniform float uRatio;

//flat out vec4 vCenter;
//flat out int vConnections;
//out vec4 vGl_Position;

in vec3 vPosition[];
out vec3 vvPosition;

bool isFront(vec3 a, vec3 b, vec3 c)
{
    float area = (a.x * b.y - b.x * a.y) + (b.x * c.y - c.x * b.y) + (c.x * a.y - a.x * c.y);
    return area > 0.0f && area < 0.1f;
}

void emitEdge(int i0, int i1)
{
    /*vec2 v = normalize(p1.xy - p0.xy);
    vec2 u = normalize(vec2(v.y, -v.x * uRatio * uRatio));
    vec2 n = u * uLineWidth;
    n = vec2(n.x, n.y * uRatio);

    float p0x_p_uLineWidth = p0.x + uLineWidth;
    float p0x_m_uLineWidth = p0.x - uLineWidth;
    float uLineWidth_x_uRatio = uLineWidth * uRatio;

    vCenter = vec4(p0.xy, p0.z, 1.0f);
    vConnections = 1;
    gl_Position = vec4(p0x_p_uLineWidth, p0.y + uLineWidth_x_uRatio, p0.z, 1.0f);
    vGl_Position = vec4(p0x_p_uLineWidth, p0.y + uLineWidth, p0.z, 1.0f);
    EmitVertex();
    gl_Position = vec4(p0x_m_uLineWidth, p0.y + uLineWidth_x_uRatio, p0.z, 1.0f);
    vGl_Position = vec4(p0x_m_uLineWidth, p0.y + uLineWidth, p0.z, 1.0f);
    EmitVertex();
    gl_Position = vec4(p0x_p_uLineWidth, p0.y - uLineWidth_x_uRatio, p0.z, 1.0f);
    vGl_Position = vec4(p0x_p_uLineWidth, p0.y - uLineWidth, p0.z, 1.0f);
    EmitVertex();
    gl_Position = vec4(p0x_m_uLineWidth, p0.y - uLineWidth_x_uRatio, p0.z, 1.0f);
    vGl_Position = vec4(p0x_m_uLineWidth, p0.y - uLineWidth, p0.z, 1.0f);
    EmitVertex();
    EndPrimitive();

    vConnections = 0;
    vCenter = vec4(p0.xy, p0.z, 1.0f);
    gl_Position = vec4(p0.xy + n, p0.z, 1.0f);
    EmitVertex();
    vCenter = vec4(p1.xy, p0.z, 1.0f);
    gl_Position = vec4(p1.xy + n, p1.z, 1.0f);
    EmitVertex();
    vCenter = vec4(p0.xy, p0.z, 1.0f);
    gl_Position = vec4(p0.xy - n, p0.z, 1.0f);
    EmitVertex();
    vCenter = vec4(p1.xy, p0.z, 1.0f);
    gl_Position = vec4(p1.xy - n, p1.z, 1.0f);
    EmitVertex();
    EndPrimitive();*/

    gl_Position = gl_in[i0].gl_Position;
    vvPosition = vPosition[i0];
    EmitVertex();
    gl_Position = gl_in[i1].gl_Position;
    vvPosition = vPosition[i1];
    EmitVertex();
    EndPrimitive();
}

void main()
{
    vec3 v0 = gl_in[0].gl_Position.xyz / gl_in[0].gl_Position.w;
    vec3 v1 = gl_in[1].gl_Position.xyz / gl_in[1].gl_Position.w;
    vec3 v2 = gl_in[2].gl_Position.xyz / gl_in[2].gl_Position.w;
    vec3 v3 = gl_in[3].gl_Position.xyz / gl_in[3].gl_Position.w;
    vec3 v4 = gl_in[4].gl_Position.xyz / gl_in[4].gl_Position.w;
    vec3 v5 = gl_in[5].gl_Position.xyz / gl_in[5].gl_Position.w;

    vec3 l0 = v0 - v1;
    vec3 l1 = v2 - v1;

    // borders of holes
    if (v3 == v2) emitEdge(0, 1);
    if (v4 == v0) emitEdge(1, 2);
    if (v5 == v1) emitEdge(2, 0);

    // filled objects
    if (isFront(v0, v2, v1)) {
        if (!isFront(v3, v0, v1)) emitEdge(0, 1);
        if (!isFront(v1, v2, v4)) emitEdge(1, 2);
        if (!isFront(v0, v5, v2)) emitEdge(2, 0);
    }
}
