#version 460 core

layout(location = 0) out vec4 FragColor;

uniform vec4 u_color = vec4(0.5f, 0.0f, 0.5f, 1.0f);

void main()
{
	FragColor = u_color;
};