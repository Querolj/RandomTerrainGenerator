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
uniform vec3 cameraFront;
uniform vec3 cameraDirection;

uniform float timePassed;

in vec3 vs_position;
in vec2 vs_texcoord;
in vec3 vs_normal;

out vec4 fs_color;



void main()
{
	float ambient_light_strenght = 1.f;
	//Ambient light
	vec3 ambientLight = ambient_light_strenght * vec3(texture(material.diffuse, vs_texcoord));
	
	//Diffuse light
	vec3 posToLightDirVec = normalize(vs_position - cameraPos);
	vec3 diffuseColor = vec3(1.f, 1.f, 1.f);
	float diffuse = clamp(dot(posToLightDirVec, vs_normal),0 ,1);
	//vec3 diffuseFinal = diffuseColor * (diffuse * material.diffuse);
	vec3 diffuseFinal = diffuseColor * diffuse * vec3(texture(material.diffuse, vs_texcoord));

	//Specular light
	vec3 light_position = cameraPos + (vec3(0.f, 0.f, -1.f));
	vec3 lightToPosDirVec = normalize(light_position - vs_position);

	vec3 reflectDirVec = normalize(reflect(lightToPosDirVec, normalize(vs_normal))); 
	vec3 posToViewDirVec = normalize(vs_position - light_position); //cameraPos at start
	float specularConstant = pow(max(dot(posToViewDirVec, reflectDirVec), 0), material.shininess) * 5.f;
	vec3 specularFinal = vec3(1.f, 1.f, 1.f) * (specularConstant * material.specular);

	float min_size = 0.5f;
	float bright_speed = 2;
	float sin_time = abs(sin(timePassed));
	float extrapolation_value = sin_time + min_size + (sin_time * -min_size);

	ambientLight *= extrapolation_value;

	fs_color = texture(texture0, vs_texcoord) *
		(vec4(ambientLight, 1.f) + vec4(diffuseFinal, 1.f) + vec4(specularFinal, 1.f) );
}