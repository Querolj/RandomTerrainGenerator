#pragma once

#include "Shader.h"
#include "libs.h"

class ShaderManager
{
public:
	ShaderManager();
	Shader* CreateShader(const GLchar* vertexPath, const GLchar* fragmentPath);
	void setMat4(string& name, glm::mat4 matrix);
private:
	vector<Shader*> _shaders;
};
