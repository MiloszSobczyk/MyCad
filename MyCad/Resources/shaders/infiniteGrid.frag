#version 460 core

layout(location = 0) in vec3 in_worldPosition;

layout(location = 0) out vec4 FragColor;

uniform float u_GridMinPixelsBetweenCells = 2.0;
uniform float u_gridCellSize = 0.1;
uniform vec4 u_gridColorThin = vec4(0.2, 0.2, 0.2, 1.0);
uniform vec4 u_gridColorThick = vec4(0.4, 0.4, 0.4, 1.0);
uniform vec4 u_CameraWorldPosition;
uniform float u_GridSize = 10000.f;

float log10(float x)
{
	float f = log(x) / log(10.0);
	return f;
}

float max2(vec2 vec)
{
	if(vec.x > vec.y)
		return vec.x;
	else 
		return vec.y;
}

void main()
{
	vec2 dvx = vec2(dFdx(in_worldPosition.x), dFdy(in_worldPosition.x));
	vec2 dvy = vec2(dFdx(in_worldPosition.z), dFdy(in_worldPosition.z));
	
	float lx = length(dvx);
	float ly = length(dvy);

	vec2 dudv = vec2(lx, ly);

	float l = length(dudv);

	float LOD = max(0.0, log10(l * u_GridMinPixelsBetweenCells / u_gridCellSize) + 1.0);

	float GridCellSizeLod0 = u_gridCellSize * pow(10.0, floor(LOD));
	float GridCellSizeLod1 = GridCellSizeLod0 * 10.0;
	float GridCellSizeLod2 = GridCellSizeLod1 * 10.0;

	dudv *= 4.0;

	vec2 mod_div_dudv = mod(in_worldPosition.xz, GridCellSizeLod0) / dudv;
	float Lod0a = max2(vec2(1.0) - abs(clamp(mod_div_dudv, 0.0, 1.0) * 2.0 - vec2(1.0)));

	mod_div_dudv = mod(in_worldPosition.xz, GridCellSizeLod1) / dudv;
	float Lod1a = max2(vec2(1.0) - abs(clamp(mod_div_dudv, 0.0, 1.0) * 2.0 - vec2(1.0)));

	mod_div_dudv = mod(in_worldPosition.xz, GridCellSizeLod2) / dudv;
	float Lod2a = max2(vec2(1.0) - abs(clamp(mod_div_dudv, 0.0, 1.0) * 2.0 - vec2(1.0)));
	
	float LOD_fade = fract(LOD);
	

	vec4 Color;

	if (Lod2a > 0.0)
	{
		Color = u_gridColorThick;
		Color.a *= Lod2a;
	} 
	else if (Lod1a > 0.0)
	{
		Color = mix(u_gridColorThick, u_gridColorThin, LOD_fade);
		Color.a *= Lod1a;
	}
	else
	{
		Color = u_gridColorThin;
		Color.a *= Lod0a * (1.0 - LOD_fade);
	}

	float OpacityFallof = (1.0 - clamp(length(in_worldPosition.xz - u_CameraWorldPosition.xz) / u_GridSize, 0.0, 1.0));

	Color.a *- OpacityFallof;

	FragColor = Color;
};