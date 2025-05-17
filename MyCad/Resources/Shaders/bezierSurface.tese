#version 460 core
layout(quads, equal_spacing, ccw) in;
uniform mat4 u_viewMatrix;
uniform mat4 u_projectionMatrix;

vec4 bernstein(float t, vec4 c0, vec4 c1, vec4 c2, vec4 c3)
{
    float it = 1.0 - t;
    return it * it * it * c0 + 3.0 * it * it * t * c1 + 3.0 * it * t * t * c2 + t * t * t * c3;
}
void main() {
    vec4 cp[4][4];
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            cp[i][j] = gl_in[i*4 + j].gl_Position;
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;
    vec4 uCurve[4];
    for (int i = 0; i < 4; ++i)
        uCurve[i] = bernstein(u, cp[i][0], cp[i][1], cp[i][2], cp[i][3]);
    vec4 pos = bernstein(v, uCurve[0], uCurve[1], uCurve[2], uCurve[3]);
    gl_Position = u_projectionMatrix * u_viewMatrix * pos;
}
