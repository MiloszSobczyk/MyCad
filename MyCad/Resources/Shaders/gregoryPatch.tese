#version 460 core

layout(triangles, equal_spacing, cw) in;

uniform mat4 u_viewMatrix;
uniform mat4 u_projectionMatrix;

void main() {
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;
    float w = gl_TessCoord.z;

    vec3 p0 = gl_in[0].gl_Position.xyz;
    vec3 p1 = gl_in[1].gl_Position.xyz;
    vec3 p2 = gl_in[2].gl_Position.xyz;
    vec3 p3 = gl_in[3].gl_Position.xyz;
    vec3 p4 = gl_in[4].gl_Position.xyz;
    vec3 p5 = gl_in[5].gl_Position.xyz;
    vec3 p6 = gl_in[6].gl_Position.xyz;
    vec3 p7 = gl_in[7].gl_Position.xyz;
    vec3 p8 = gl_in[8].gl_Position.xyz;
    vec3 p9 = gl_in[9].gl_Position.xyz;

    float B300 = u * u * u;
    float B030 = v * v * v;
    float B003 = w * w * w;
    float B210 = 3.0 * u * u * v;
    float B120 = 3.0 * u * v * v;
    float B021 = 3.0 * v * v * w;
    float B012 = 3.0 * v * w * w;
    float B102 = 3.0 * u * w * w;
    float B201 = 3.0 * u * u * w;
    float B111 = 6.0 * u * v * w;

    vec3 pos =
          p0 * B300
        + p1 * B210
        + p2 * B120
        + p3 * B030
        + p4 * B021
        + p5 * B012
        + p6 * B003
        + p7 * B102
        + p8 * B201
        + p9 * B111;

    gl_Position = u_projectionMatrix * u_viewMatrix * vec4(pos, 1.0);
}
