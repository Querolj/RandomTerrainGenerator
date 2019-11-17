#include "ShaderManager.h"


ShaderManager::ShaderManager()
{

}

Shader* ShaderManager::CreateShader(const GLchar* vertexPath, const GLchar* fragmentPath)
{
	Shader* shader = new Shader(vertexPath, fragmentPath);
	_shaders.push_back(shader);

	return shader;
}



void ShaderManager::setMat4(string& name, glm::mat4 matrix)
{
	int i;
	for (i = 0; i < _shaders.size(); i++)
	{
		(*_shaders[i]).setMat4(name, matrix);
	}

}
