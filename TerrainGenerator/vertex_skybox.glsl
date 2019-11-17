#version 440 core
layout (location = 0) in vec3 vertex_position;

out vec3 vs_texcoord;

uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;
void main()
{
    vs_texcoord = vertex_position;
	vec4 pos = ProjectionMatrix * ViewMatrix * vec4(vertex_position, 1.0);
    gl_Position = pos.xyww;
}  