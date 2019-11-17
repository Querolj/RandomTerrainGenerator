#version 440

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec2 vertex_texcoord;
layout (location = 2) in vec3 vertex_normal;

out vec3 vs_position;
out vec2 vs_texcoord;
out vec3 vs_normal;
out vec4 world_position_matrix;
out vec4 frag_position_light_space;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 lightSpaceMatrix;

uniform float TimePassed;

//Terrain properties
uniform float HeightMultiplier;

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

void main()
{
	vs_position = vec4(ModelMatrix * vec4(vertex_position, 1.f)).xyz;
	//vs_position.y = softmax(vs_position.y, 5.f, 3, 0.f);

	vs_texcoord = vec2(vertex_texcoord.x, vertex_texcoord.y );
	vs_normal = mat3(ModelMatrix) * vertex_normal;


	frag_position_light_space = lightSpaceMatrix * vec4(vertex_position, 1.f);

	
	float min_size = 0.92f;
	world_position_matrix =  vec4((vertex_position ),1.f );
	gl_Position = ProjectionMatrix  * ViewMatrix * ModelMatrix * vec4(vertex_position, 1.f);
}