#version 460 core

layout(vertices = 10) out;

uniform int u_tessLevel = 12;

void main() {
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

    float lvl = float(u_tessLevel);
    gl_TessLevelOuter[0] = lvl;
    gl_TessLevelOuter[1] = lvl;
    gl_TessLevelOuter[2] = lvl;
    gl_TessLevelInner[0] = lvl;
}
