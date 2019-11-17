#pragma once
#include<vec3.hpp>

class Material {
public:
	Material();

	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;
};