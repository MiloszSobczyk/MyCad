#version 460 core

uniform vec4 u_Color = vec4(0.8, 0.8, 0.8, 1);

out vec4 FragColor;

void main() 
{
    FragColor = u_Color;
}
