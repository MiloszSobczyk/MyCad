#version 460 core

layout(isolines, equal_spacing, ccw) in;

uniform mat4 u_viewMatrix;
uniform mat4 u_projectionMatrix;

void main()
{
    vec4 p0 = gl_in[0].gl_Position;
    vec4 p1 = gl_in[1].gl_Position;
    vec4 p2 = gl_in[2].gl_Position;
    vec4 p3 = gl_in[3].gl_Position;

    float t = gl_TessCoord.x;

    gl_Position = u_projectionMatrix * u_viewMatrix * ((1 - t) * p0 + t * p1);
}
