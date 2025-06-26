#version 460 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;

uniform mat4 u_projectionMatrix;
uniform mat4 u_viewMatrix;
uniform mat4 u_modelMatrix;

out vec4 v_color;

void main()
{
    gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix * position;
    v_color = color;
}
