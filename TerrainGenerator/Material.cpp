#include "Material.h"

Material::Material()
{
	diffuse = glm::vec3(1.f);
	specular = glm::vec3(1.f);
	shininess = 32.f;
}
