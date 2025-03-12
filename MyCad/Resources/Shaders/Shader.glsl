#shader vertex
#version 410 core

layout(location = 0) in vec4 a_Position;
layout(location = 1) in vec4 a_Color;

layout(location = 0) out vec4 out_Color;

void main() {
    gl_Position = a_Position;
    out_Color = a_Color;
}

#shader fragment
#version 410 core

layout(location = 0) in vec4 out_Color;
out vec4 FragColor;

void main() {
    FragColor = vec4(0.2, 0.2, 0.0, 0.5) + out_Color;
}
