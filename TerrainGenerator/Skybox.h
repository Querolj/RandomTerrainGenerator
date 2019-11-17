#pragma once

#include "libs.h";
#include "TextureManager.h";
#include "Shader.h";
#include "Camera.h";

float skybox_vertices[] = {
	// positions          
	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f
};

vector<std::string> faces
{
		"Images/cubemaps/skybox02/front.png",
		"Images/cubemaps/skybox02/back.png",
		
		"Images/cubemaps/skybox02/top.png",
		"Images/cubemaps/skybox02/bottom.png",

		"Images/cubemaps/skybox02/right.png",
		"Images/cubemaps/skybox02/left.png"
		
		
};

class Skybox
{
public:
	GLuint cubemapID;

	Skybox();
	void Draw(Shader* shader, Camera& camera);

private:
	glm::mat4 _model_matrix;
	
	//VAO, VBO...
	unsigned int VAO, VBO;
	void setupSkyboxMesh();
};

Skybox::Skybox() 
{
	this->_model_matrix = glm::mat4(1.f);
	cubemapID = TextureManager::loadCubemap(faces);

	setupSkyboxMesh();
}

void Skybox::Draw(Shader* shader, Camera& camera)
{
	glDepthFunc(GL_LEQUAL);
	shader->use();
	glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
	shader->setMat4("ViewMatrix", view);


	shader->use();

	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapID);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS);
}

void Skybox::setupSkyboxMesh()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices), &skybox_vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

}