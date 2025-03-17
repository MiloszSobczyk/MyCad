#version 460 core

layout(location = 0) in vec4 in_position;

layout(location = 0) out vec3 out_worldPosition;

uniform mat4 u_projectionMatrix;
uniform mat4 u_viewMatrix;
uniform vec4 u_CameraWorldPosition;

uniform float u_GridSize = 10000.f;

void main()
{
    vec3 position = in_position.xyz * u_GridSize;
    position.x += u_CameraWorldPosition.x;
    position.z += u_CameraWorldPosition.z;

    mat4 mvp = u_projectionMatrix * u_viewMatrix;
    vec4 pos = mvp * vec4(position, 1.0);

    gl_Position = pos;
    out_worldPosition = position;
};