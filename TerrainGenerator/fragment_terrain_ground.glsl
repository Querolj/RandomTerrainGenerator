#version 440
struct Material {
	sampler2D diffuse;
	vec3 specular;
	float shininess;
};
uniform Material material;

//Every textures
uniform sampler2D herb_text;
uniform sampler2D rock01_text;
uniform sampler2D rock02_text;

//Terrain properties
uniform float HeightMultiplier;

//Shadow
uniform sampler2D shadowMap;

//Camera variables
uniform vec3 CameraPos;

//Light
uniform vec3 DiffuseLightPosition;


uniform float TimePassed;

in vec3 vs_position;
in vec2 vs_texcoord;
in vec3 vs_normal;
in vec4 world_position_matrix;
in vec4 frag_position_light_space;


out vec4 fs_color;

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

float random (vec2 st) {
    return fract(sin(dot(st.xy,
                         vec2(12.9898,78.233)))*
        43758.5453123);
}

float ShadowCalculation(vec4 frag_pos_light_space)
{
	vec3 proj_coords = frag_pos_light_space.xyz / frag_pos_light_space.w;
	proj_coords = proj_coords * 0.5 + 0.5; 
	float closest_depth = texture(shadowMap, proj_coords.xy).r; 
	float current_depth = proj_coords.z;  

	float shadow = current_depth > closest_depth  ? 1.0 : 0.0;
	
	return shadow;
}


void main()
{
	vec3 terrain_color = vec3(139.f,69.f,19.f) / 255.f;
	//test light position
	vec3 diffuse_light_position = DiffuseLightPosition;
	diffuse_light_position.x = CameraPos.x;
	diffuse_light_position.z = CameraPos.z;

	vec3 color_herb = texture(herb_text, vs_texcoord).rgb * 0.7f;
	vec3 color_rock01 = (texture(rock02_text, vs_texcoord).rgb + terrain_color) * 0.5f;//texture(rock01_text, vs_texcoord).rgb;
	vec3 color_rock02 = texture(rock02_text, vs_texcoord).rgb + terrain_color;

	//Textures interpolation
	float height = smoothstep(-HeightMultiplier, HeightMultiplier, vs_position.y);
	height = softmax(height, 1.f, 3, 0.09f);

	float interp_height = clamp(height, 0.f, 1.f);

	vec3 final_texture = mix(color_herb, color_rock02, interp_height);
	final_texture = mix(final_texture, color_rock01, height);
	//vec3 final_texture = terrain_color;

	//LIGHT

	float ambient_light_strenght = 0.5f;
	//Ambient light
	vec3 ambientLight = ambient_light_strenght * final_texture;
	
	//Diffuse light
	vec3 posToLightDirVec = normalize(vs_position - DiffuseLightPosition);
	vec3 diffuseColor = vec3(1.f, 1.f, 1.f);
	float diffuse = clamp(dot(posToLightDirVec, vs_normal),0 ,1);
	vec3 diffuseFinal = diffuseColor * diffuse;

	//Specular light
	vec3 light_position = CameraPos + (vec3(0.f, -10.f, 0.f));
	vec3 lightToPosDirVec = normalize(light_position - vs_position);

	vec3 reflectDirVec = normalize(reflect(lightToPosDirVec, normalize(vs_normal))); 
	vec3 posToViewDirVec = normalize(vs_position - light_position); //CameraPos at start
	float specularConstant = pow(max(dot(posToViewDirVec, reflectDirVec), 0), material.shininess);
	vec3 specularFinal = final_texture * (specularConstant * material.specular);

	//Shadow calculation
	float shadow = ShadowCalculation(frag_position_light_space);       
    vec3 lighting = final_texture * (ambientLight + (shadow) * (diffuseFinal + specularFinal)); 
	
	fs_color = vec4(lighting, 1.0);
}