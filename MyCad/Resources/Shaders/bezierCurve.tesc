#version 460 core

layout(vertices = 4) out;
out patch float maxU;

uniform mat4 u_viewMatrix;
uniform mat4 u_projectionMatrix;
uniform vec4 u_cameraPos;
uniform vec4 u_zoomLevel;

float screenDistance(vec4 a, vec4 b)
{
    vec4 clipA = u_projectionMatrix * u_viewMatrix * a;
    vec4 clipB = u_projectionMatrix * u_viewMatrix * b;

    vec3 ndcA = clipA.xyz / clipA.w;
    vec3 ndcB = clipB.xyz / clipB.w;

    return length(ndcA.xy - ndcB.xy);
}

float averageDistanceToCamera()
{
    vec3 worldPos0 = gl_in[0].gl_Position.xyz;
    vec3 worldPos1 = gl_in[1].gl_Position.xyz;
    vec3 worldPos2 = gl_in[2].gl_Position.xyz;
    vec3 worldPos3 = gl_in[3].gl_Position.xyz;

    vec3 camPos = u_cameraPos.xyz;

    float d0 = distance(worldPos0, camPos);
    float d1 = distance(worldPos1, camPos);
    float d2 = distance(worldPos2, camPos);
    float d3 = distance(worldPos3, camPos);

    return (d0 + d1 + d2 + d3) * 0.25;
}

void main()
{
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

    float d0 = screenDistance(gl_in[0].gl_Position, gl_in[1].gl_Position);
    float d1 = screenDistance(gl_in[1].gl_Position, gl_in[2].gl_Position);
    float d2 = screenDistance(gl_in[2].gl_Position, gl_in[3].gl_Position);
    float d3 = screenDistance(gl_in[3].gl_Position, gl_in[0].gl_Position);

    float baseScale = 32.0;
    
    float distToCamera = averageDistanceToCamera();
    
    float distanceFactor = clamp(100.0 / distToCamera, 0.5, 2.0);

    float scale = baseScale * distanceFactor * u_zoomLevel.x;
    float part = d1 * scale;

    gl_TessLevelOuter[0] = ceil(sqrt(part));
    gl_TessLevelOuter[1] = ceil(sqrt(part));

    maxU = ceil(sqrt(part));
}
