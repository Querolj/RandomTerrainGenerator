#version 440

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec2 vertex_texcoord;
layout (location = 2) in vec3 vertex_normal;

out vec3 vs_position;
out vec3 vs_old_position;
out vec2 vs_texcoord;
out vec3 vs_normal;
out vec4 world_position_matrix;
out vec4 frag_position_light_space;
out float terrain_height;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 lightSpaceMatrix;

uniform float TimePassed;
uniform float DeltaTime;

//water properties
uniform float WavesAmplitude;
uniform float WavesSpeed;
uniform int Dimension;

//Perlin noise
uniform sampler2D main_water_texture;
uniform sampler2D foam_texture;
uniform sampler2D perlin_noise;
uniform sampler2D terrain_perlin_noise;


float softmax(float value, float max ,int iteration, float offset)
{
	value += offset;

	for(int i=0; i<iteration; i++)
	{
		value = exp(value);
		max = exp(max);
	}

	value = (value/max);
	value = clamp(value, 0.f, 1.f);

	return value;
}

float calculateWaveHeight(vec3 v_pos)
{
	//float sin_time = abs(sin(ajusted_time + angle));
	//float wave_height = sin_time + min_size + (sin_time * -min_size);

	int sq_dimension = Dimension * Dimension;

	float lerp_x = v_pos.x + TimePassed * WavesSpeed;
	float lerp_z = v_pos.z + TimePassed * WavesSpeed;
	lerp_x -= floor(lerp_x);
	lerp_z -= floor(lerp_z);

	int x = int( floor(lerp_x) ) % (sq_dimension);
	int z = int( floor(lerp_z) ) % (sq_dimension);
	x = int( floor(v_pos.x + (TimePassed * WavesSpeed)) ) % (sq_dimension);
	z = int( floor(v_pos.z + (TimePassed * WavesSpeed)) ) % (sq_dimension);
	float noise = float( texture2D(perlin_noise, (vec2(x, z) + vec2(lerp_x, lerp_z)) / Dimension ) );
	noise = sin(noise);
	noise = softmax(noise, 1.f, 2, 0.f);

	return sin(noise);
}

float calculateTerrainHeight(vec3 v_pos, vec2 offset)
{
	float terrain_height;
	
	terrain_height = float( texture2D(terrain_perlin_noise, (vec2(v_pos.z, v_pos.x) + offset) / Dimension  ) );

	return terrain_height;
}

vec3 calculateNormal(vec3 v1, vec3 v2, vec3 v3)
{
	vec3 normal;
	vec3 u = normalize(v2 - v1);
	vec3 v = normalize(v3 - v1);

	normal.x = (u.y * v.z) - (u.z * v.y);
	normal.y = (u.z * v.x) - (u.x * v.z);
	normal.z = (u.x * v.y) - (u.y * v.x);

	return normalize(normal);
}

void main()
{
	//Calculate wave height for a triangle
	float wave_height1 = calculateWaveHeight(vertex_position); 
	vec3 new_vertex_position = vertex_position;
	new_vertex_position.y = wave_height1;

	vec3 neighbour1 = vertex_position;
	neighbour1.x = vertex_position.x + 1;
	float wave_height2 = calculateWaveHeight(neighbour1); 
	neighbour1.y = wave_height2;

	vec3 neighbour2 = vertex_position;
	neighbour2.z = vertex_position.y + Dimension;
	float n2_z = (int( floor(neighbour2.z) ) % Dimension) + (neighbour2.z - floor(neighbour2.z));
	neighbour2.z = n2_z;
	float wave_height3 = calculateWaveHeight(neighbour2); 
	neighbour2.y = wave_height3;

	//Calculate normal
	vec3 new_vertex_normal = calculateNormal(new_vertex_position, neighbour1, neighbour2);

	vs_position = vec4(ModelMatrix * vec4(new_vertex_position, 1.f)).xyz;
	vs_texcoord = vec2(vertex_texcoord.x, vertex_texcoord.y );

	vs_normal = mat3(ModelMatrix) * new_vertex_normal;
	
	terrain_height = calculateTerrainHeight(vertex_position, vec2(0.5f, 0.5f) );

	gl_Position = ProjectionMatrix  * ViewMatrix * ModelMatrix * vec4(new_vertex_position, 1.f);
}