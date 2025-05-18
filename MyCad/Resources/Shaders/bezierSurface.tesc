#version 460 core

layout(vertices = 16) out;

uniform int u_tessLevelU;
uniform int u_tessLevelV;

void main() 
{
    if (gl_InvocationID == 0) 
    {
        float tU = max(float(u_tessLevelU), 1.0);
        float tV = max(float(u_tessLevelV), 1.0);

        gl_TessLevelOuter[0] = tU;
        gl_TessLevelOuter[2] = tU;
        gl_TessLevelOuter[1] = tV;
        gl_TessLevelOuter[3] = tV;

        gl_TessLevelInner[0] = tU;
        gl_TessLevelInner[1] = tV;
    }

    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}
