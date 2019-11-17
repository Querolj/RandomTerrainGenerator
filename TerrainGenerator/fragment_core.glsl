#version 440
struct Material {
	sampler2D diffuse;
	vec3 specular;
	float shininess;
};
uniform Material material;

//Every textures
uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D texture2;



//Camera variables
uniform vec3 cameraPos;

uniform float timePassed;

in vec3 vs_position;
in vec2 vs_texcoord;
in vec3 vs_normal;


out vec4 fs_color;



void main()
{
	vec3 final_texture = vec3(0.3f, 0.63f, 0.3f);


	float ambient_light_strenght = 1.f;
	//Ambient light
	vec3 ambientLight = ambient_light_strenght * final_texture;
	
	//Diffuse light
	vec3 posToLightDirVec = normalize(vs_position - cameraPos);
	vec3 diffuseColor = vec3(1.f, 1.f, 1.f);
	float diffuse = clamp(dot(posToLightDirVec, vs_normal),0 ,1);
	//vec3 diffuseFinal = diffuseColor * (diffuse * material.diffuse);
	vec3 diffuseFinal = diffuseColor * diffuse * vec3(texture(material.diffuse, vs_texcoord));

	//Specular light
	vec3 light_position = cameraPos + (vec3(0.f, -10.f, 0.f));
	vec3 lightToPosDirVec = normalize(light_position - vs_position);

	vec3 reflectDirVec = normalize(reflect(lightToPosDirVec, normalize(vs_normal))); 
	vec3 posToViewDirVec = normalize(vs_position - light_position); //cameraPos at start
	float specularConstant = pow(max(dot(posToViewDirVec, reflectDirVec), 0), material.shininess);
	vec3 specularFinal = final_texture * (specularConstant * material.specular);

	fs_color = vec4(final_texture, 1.f) * 
		(vec4(ambientLight, 1.f)) + vec4(diffuseFinal, 1.f) + vec4(specularFinal, 1.f);
	

}