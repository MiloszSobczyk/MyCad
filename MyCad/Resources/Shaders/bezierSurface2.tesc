#version 460

layout (vertices=16) out;

uniform int u_tessLevelU;
uniform int u_tessLevelV;

void main() 
{
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    
    gl_TessLevelOuter[0] = ceil(u_tessLevelV + 1);
    gl_TessLevelOuter[1] = ceil(u_tessLevelU);
}