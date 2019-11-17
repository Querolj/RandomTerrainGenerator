#version 440 core
out vec4 frag_color;

in vec3 vs_texcoord;

uniform samplerCube skybox;

void main()
{    
    frag_color = texture(skybox, vs_texcoord);
}