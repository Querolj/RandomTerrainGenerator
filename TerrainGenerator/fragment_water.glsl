#version 440
struct Material {
	sampler2D diffuse;
	vec3 specular;
	float shininess;
};
uniform Material material;

uniform sampler2D main_water_texture;
uniform sampler2D foam_texture;
uniform sampler2D perlin_noise;
uniform sampler2D terrain_perlin_noise;
//Reflection
uniform samplerCube skybox;

//Camera variables
uniform vec3 CameraPos;

//Light
uniform vec3 DiffuseLightPosition;

uniform float TimePassed;

in vec3 vs_position;
in vec3 vs_old_position;
in vec2 vs_texcoord;
in vec3 vs_normal;
in float terrain_height;

out vec4 fs_color;

//water properties
uniform float WavesAmplitude;
uniform float WavesSpeed;
uniform int Dimension;



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
	float lerp_text = smoothstep(WavesAmplitude, 1.f, vs_position.y);
	lerp_text = softmax(lerp_text, 1.f, 2, 0.f);

	vec3 color_main_water = texture(main_water_texture, vs_texcoord).rgb;
	vec3 color_foam = texture(foam_texture, vs_texcoord).rgb;
	vec3 final_texture = mix(color_main_water, color_foam, lerp_text);

	//Foam near coast
	float terrain_height_clamped = clamp(terrain_height + 1.f, 0.f, 1.f);
	final_texture = mix(final_texture, color_foam, terrain_height_clamped);
	
	//Reflection
	vec3 I = normalize(vs_position - CameraPos);
	vec3 R = reflect(I, normalize(vs_normal));
	vec3 reflection = texture(skybox, R).rgb;

	//LIGHT
	//Ambient
	float ambient_light_strenght = 1.f;
	vec3 ambientLight = ambient_light_strenght * final_texture;

	//Diffuse
	vec3 posToLightDirVec = normalize(vs_position - DiffuseLightPosition);
	vec3 diffuseColor = vec3(1.f, 1.f, 1.f);
	float diffuse = clamp(dot(posToLightDirVec, vs_normal),0 ,1);
	vec3 diffuseFinal = diffuseColor * diffuse * vec3(texture(material.diffuse, vs_texcoord));
	
	//Specular 
	vec3 light_position = CameraPos;
	vec3 lightToPosDirVec = normalize(light_position - vs_position);

	vec3 reflectDirVec = normalize(reflect(lightToPosDirVec, normalize(vs_normal))); 
	vec3 posToViewDirVec = normalize(vs_position - light_position); //CameraPos at start
	float specularConstant = pow(max(dot(posToViewDirVec, reflectDirVec), 0), material.shininess);
	vec3 specularFinal = final_texture * (specularConstant * material.specular);

    vec3 lighting = final_texture * reflection * (ambientLight + diffuseFinal + specularFinal); 
	//lighting *= reflection;

	fs_color = vec4(lighting, 1.0);
	fs_color.a = mix(1.f, 0.75f, lerp_text);
}  