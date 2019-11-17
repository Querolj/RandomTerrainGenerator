#pragma once
#include "VerticesStruct.h"
#include "Shader.h"

class Mesh {
public:
	/*  Mesh Data  */
	vector<Vertex> _vertices;
	vector<unsigned int> _indices;
	vector<Texture> _textures;
	/*  Functions  */
	Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);
	void Draw(Shader* shader);
private:
	/*  Render data  */
	unsigned int VAO, VBO, EBO;
	/*  Functions    */
	void setupMesh();

};

Mesh::Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
{
	this->_vertices = vertices;
	this->_indices = indices;
	this->_textures = textures;

	setupMesh();
}

void Mesh::setupMesh()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(Vertex), &_vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(unsigned int),
		&_indices[0], GL_STATIC_DRAW);

	// vertex positions
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);
	// vertex texture coords
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texcoord));
	glEnableVertexAttribArray(1);
	// vertex normals
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
}

void Mesh::Draw(Shader* shader)
{
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	for (unsigned int i = 0; i < _textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i); // activate proper texture unit before binding
		// retrieve texture number (the N in diffuse_textureN)
		string number;
		string name = _textures[i].type;
		if (name == "texture_diffuse")
			number = std::to_string(diffuseNr++);
		else if (name == "texture_specular")
			number = std::to_string(specularNr++);

		shader->setFloat(("material." + name + number).c_str(), i);

		glBindTexture(GL_TEXTURE_2D, _textures[i].id);
	}
	glActiveTexture(GL_TEXTURE0);

	// draw mesh
	GLuint program = shader->getProgram();
	glUseProgram(program);
	glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glUseProgram(0);

}