#version 460 core

layout(vertices = 16) out;

void main() 
{
    if (gl_InvocationID == 0) {
        float t = 18;
        gl_TessLevelOuter[0] = t;
        gl_TessLevelOuter[1] = t;
        gl_TessLevelOuter[2] = t;
        gl_TessLevelOuter[3] = t;
        gl_TessLevelInner[0] = t;
        gl_TessLevelInner[1] = t;
    }
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}
