#pragma once
#ifndef SHADER_H
#define SHADER_H

#include "libs.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdio.h>

class Shader
{
public:
	// the program ID
	unsigned int ID;

	// constructor reads and builds the shader
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
	//Get shader program
	GLuint getProgram();
	//Use shader program
	void use();
	// utility uniform functions
	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setMat4(const string& name, glm::mat4 matrix) const;
	void setVec3(const string& name, glm::vec3 vector) const;
	void setFloatArray(const string& name, float *array, int size) const;

	//Material stuff
	void setDiffuseLightColor(glm::vec3 diffuse_color);
	void setSpecularLightColor(glm::vec3 specular_color);
	void setShininess(float shininiess);

	//Shader constant
private:
	void initMaterial(); //Set material at a value by default

};

#endif