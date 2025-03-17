#shader vertex
#version 410 core

layout(location = 0) in vec4 position;

uniform mat4 u_projectionMatrix;
uniform mat4 u_viewMatrix;

void main()
{
    gl_Position = u_projectionMatrix * u_viewMatrix * position;
};

#shader fragment
#version 410 core

layout(location = 0) out vec4 color;

void main()
{
    color = vec4(1.f, 0.2f, 0.f, 1.f);
};