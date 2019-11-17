#pragma once
#include "libs.h";

struct Texture {
	unsigned int id;
	string type;
	string path;
};



struct Vertex
{
	glm::vec3 position;
	glm::vec2 texcoord;
	glm::vec3 normal;

};



const Vertex cube[] =
{
	//Position							//texture coordonates	//Normal
	glm::vec3(0.5f, 0.5f, 0.f),			glm::vec2(0.5f, 0.5f),	glm::vec3(0.f, 0.f, -1.f),
	glm::vec3(0.5f, -0.5f, 0.f),		glm::vec2(0.5f, 0.f),	glm::vec3(0.f, 0.f, -1.f),
	glm::vec3(-0.5f, -0.5f, 0.f),		glm::vec2(0.f, 0.f),	glm::vec3(0.f, 0.f, -1.f),
	glm::vec3(-0.5f, 0.5f, 0.f),		glm::vec2(0.f, 0.5f),	glm::vec3(0.f, 0.f, -1.f),

	glm::vec3(-0.5f, -0.5f, 1.f),		glm::vec2(0.f, 0.f),	glm::vec3(0.f, 0.f, 1.f),
	glm::vec3(0.5f, -0.5f, 1.f),		glm::vec2(0.5f, 0.f),	glm::vec3(0.f, 0.f, 1.f),
	glm::vec3(0.5f, 0.5f, 1.f),			glm::vec2(0.5f, 0.5f),	glm::vec3(0.f, 0.f, 1.f),
	glm::vec3(-0.5f, 0.5f, 1.f),		glm::vec2(0.f, 0.5f),	glm::vec3(0.f, 0.f, 1.f),

	glm::vec3(-0.5f, -0.5f, 1.f),		glm::vec2(0.f, 0.5f),	glm::vec3(0.f, -1.f, 0.f),
	glm::vec3(-0.5f, -0.5f, 0.f),		glm::vec2(0.f, 0.f),	glm::vec3(0.f, -1.f, 0.f),
	glm::vec3(0.5f, -0.5f, 0.f),		glm::vec2(0.5f, 0.f),	glm::vec3(0.f, -1.f, 0.f),
	glm::vec3(0.5f, -0.5f, 1.f),		glm::vec2(0.5f, 0.5f),	glm::vec3(0.f, -1.f, 0.f),

	glm::vec3(-0.5f, 0.5f, 0.f),		glm::vec2(0.f, 0.5f),	glm::vec3(0.f, 1.f, 0.f),
	glm::vec3(-0.5f, 0.5f, 1.f),		glm::vec2(0.f, 0.f),	glm::vec3(0.f, 1.f, 0.f),
	glm::vec3(0.5f, 0.5f, 1.f),			glm::vec2(0.5f, 0.f),	glm::vec3(0.f, 1.f, 0.f),
	glm::vec3(0.5f, 0.5f, 0.f),			glm::vec2(0.5f, 0.5f),	glm::vec3(0.f, 1.f, 0.f),

	glm::vec3(-0.5f, 0.5f, 0.f),		glm::vec2(0.f, 0.5f),	glm::vec3(-1.f, 0.f, 0.f),
	glm::vec3(-0.5f, -0.5f, 0.f),		glm::vec2(0.f, 0.f),	glm::vec3(-1.f, 0.f, 0.f),
	glm::vec3(-0.5f, -0.5f, 1.f),		glm::vec2(0.5f, 0.f),	glm::vec3(-1.f, 0.f, 0.f),
	glm::vec3(-0.5f, 0.5f, 1.f),		glm::vec2(0.5f, 0.5f),	glm::vec3(-1.f, 0.f, 0.f),

	glm::vec3(0.5f, 0.5f, 1.f),			glm::vec2(0.f, 0.5f),	glm::vec3(1.f, 0.f, 0.f),
	glm::vec3(0.5f, -0.5f, 1.f),		glm::vec2(0.f, 0.f),	glm::vec3(1.f, 0.f, 0.f),
	glm::vec3(0.5f, -0.5f, 0.f),		glm::vec2(0.5f, 0.f),	glm::vec3(1.f, 0.f, 0.f),
	glm::vec3(0.5f, 0.5f, 0.f),			glm::vec2(0.5f, 0.5f),	glm::vec3(1.f, 0.f, 0.f)
};

const GLuint indices[] =
{
	0, 1, 2,
	3, 0, 2,

	4, 5, 6,
	7, 4, 6,

	8, 9, 10,
	11, 8, 10,

	12, 13, 14,
	15, 12, 14,

	16, 17, 18,
	19, 16, 18,

	20, 21, 22,
	23, 20, 22
};


//Testing purpose variables
//glm::vec3 cubePositions[] = {
//  glm::vec3(0.0f,  0.0f,  0.0f),
//  glm::vec3(2.0f,  5.0f, -15.0f),
//  glm::vec3(-1.5f, -2.2f, -2.5f),
//  glm::vec3(-3.8f, -2.0f, -12.3f),
//  glm::vec3(2.4f, -0.4f, -3.5f),
//  glm::vec3(-1.7f,  3.0f, -7.5f),
//  glm::vec3(1.3f, -2.0f, -2.5f),
//  glm::vec3(1.5f,  2.0f, -2.5f),
//  glm::vec3(1.5f,  0.2f, -1.5f),
//  glm::vec3(-1.3f,  1.0f, -1.5f)
//};