#pragma once
#include "VerticesStruct.h"
#include "Material.h";
#include "Shader.h";
#include "TextureManager.h";

class Water
{
public:
	Water();
	Water(int dimension, shared_ptr<Material> material, Shader* shader, float mesh_precision, float* terrain_perlin_noise);
	~Water();
	void Draw(Shader* shader);
	//use to check input, debug mode
	void updateInput(GLFWwindow* window);
	void generateWater(Shader* shader, float* terrain_noise);

	GLuint _main_water_text;
	GLuint _foam_text;
	GLuint _perlin_noise_text;
	GLuint _terrain_perlin_noise_text;
	GLuint _skybox;
	
	//water properties
	float _mesh_precision = 5.f;
	float _water_height = 0.f;
	float _waves_amplitude = 0.6f;
	float _waves_speed = 1.f;
	int _dimension;

private:
	glm::mat4 _model_matrix;
	//Render data
	unsigned int VAO, VBO, EBO;
	bool _wireframe = false;

	//vertex data, constains vertex position, text coord and normal
	vector<Vertex> _vertices;
	vector<unsigned int> _indices;
	shared_ptr<Material> _water_material;

	//input variables
	bool _one_pressed = false;
	
	//Perlin
	glm::vec2* _noise_seed_2d = nullptr;
	float* _perlin_noise; //Perlin noise for water waves
	float* _terrain_perlin_noise; //terrain's perlin noise, use to get foam near coast
	void generatePerlinNoise(Shader* shader);
	void generateRandomData();
	float lerp(float a0, float a1, float w);

	//Mesh setup
	void generateWaterMesh(); //build water meshs
	void setupWaterMesh();
};

Water::Water() {}
Water::Water(int dimension, shared_ptr<Material> material, Shader* shader, float mesh_precision, float* terrain_perlin_noise)
{
	this->_model_matrix = glm::mat4(1.f);
	this->_dimension = dimension;
	this->_water_material = material;
	this->_mesh_precision = mesh_precision;
	this->_terrain_perlin_noise = terrain_perlin_noise;

	generateWaterMesh();
	generatePerlinNoise(shader);
}

void Water::generateWaterMesh()
{
	glm::vec3 default_normal(0.f, 1.f, 0.f);
	int dimension_precision = ceil(_dimension * _mesh_precision);

	int index = 0;
	for (int x = 0; x < dimension_precision; x++)
	{
		for (int y = 0; y < dimension_precision; y++)
		{
			float pos_x = (float)x / _mesh_precision;
			float pos_y = (float)y / _mesh_precision;

			//push vertex
			glm::vec3 vertex_position = glm::vec3(pos_x, _water_height, pos_y);
			Vertex vertex = { vertex_position, glm::vec2(pos_x, pos_y), default_normal};
			_vertices.push_back(vertex);

			//push indice

			if (x == 0) //No indice set on the first line
			{
				index++;
				continue;
			}

			if (y != _dimension) // right triangle
			{
				_indices.push_back(index);
				_indices.push_back(index - dimension_precision - 1);
				_indices.push_back(index - dimension_precision);
			}

			if (y != 0) //left triangle
			{
				_indices.push_back(index);
				_indices.push_back(index - 1);
				_indices.push_back(index - dimension_precision - 1);
			}
			index++;
		}
	}
	setupWaterMesh();
}

void Water::generateRandomData()
{
	int n_sq = _dimension * _dimension;
	_noise_seed_2d = new glm::vec2[n_sq + 2];

	for (int i = 0; i <= n_sq; i++)
	{
		float r1 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		float r2 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		r1 = (r1 * 4) - 2;
		r2 = (r2 * 4) - 2;

		_noise_seed_2d[i] = glm::vec2(r1, r2);
	}
}

void Water::generatePerlinNoise(Shader* shader)
{
	generateRandomData();

	_perlin_noise = new float[_dimension * _dimension];// (float*) malloc(sizeof(float) * _dimension * _dimension);
	int seed_len = _dimension * _dimension;

	for (int x = 0; x < _dimension; x++)
	{
		for (int y = 0; y < _dimension; y++)
		{
			float ajusted_pos_x = x * 0.4f;
			float ajusted_pos_y = y * 0.02f;

			glm::vec2 vertex_position_2d(ajusted_pos_x, ajusted_pos_y);

			//dot
			int x0 = floor(ajusted_pos_x);
			int y0 = floor(ajusted_pos_y);
			int x1 = ceil(ajusted_pos_x);
			int y1 = ceil(ajusted_pos_y);

			glm::vec2 dir1 = glm::vec2(x0, y0) - vertex_position_2d;
			glm::vec2 dir2 = glm::vec2(x1, y0) - vertex_position_2d;
			glm::vec2 dir3 = glm::vec2(x0, y1) - vertex_position_2d;
			glm::vec2 dir4 = glm::vec2(x1, y1) - vertex_position_2d;

			float dot1 = glm::dot(dir1, _noise_seed_2d[(x0 * _dimension + y0) % seed_len]);
			float dot2 = glm::dot(dir2, _noise_seed_2d[(x1 * _dimension + y0) % seed_len]);
			float dot3 = glm::dot(dir3, _noise_seed_2d[(x0 * _dimension + y1) % seed_len]);
			float dot4 = glm::dot(dir4, _noise_seed_2d[(x1 * _dimension + y1) % seed_len]);

			float sx = ajusted_pos_x - (float)x0;
			float sy = ajusted_pos_y - (float)y0;

			float lerp1 = lerp(dot1, dot2, sx);
			float lerp2 = lerp(dot3, dot4, sx);

			float height = lerp(lerp1, lerp2, sy);
			_perlin_noise[x * _dimension + y] = height * 2;
			//cout << (x * _dimension + y) << " : " << _perlin_noise[x * _dimension + y] << endl;
		}
	}
	//Send perlin noise to shader
	TextureManager::setupFloatTexture(_perlin_noise_text, _perlin_noise, _dimension, _dimension);
	//Send terrain perlin noise to shader
	TextureManager::setupFloatTexture(_terrain_perlin_noise_text, _terrain_perlin_noise, _dimension, _dimension);

	shader->setInt("Dimension", _dimension);

}

void Water::generateWater(Shader* shader, float* terrain_noise)
{
	_terrain_perlin_noise = terrain_noise;
	generatePerlinNoise(shader);
}

void Water::setupWaterMesh()
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

void Water::Draw(Shader* shader)
{
	shader->setFloat("WavesAmplitude", _waves_amplitude);
	shader->setFloat("WavesSpeed", _waves_speed);

	shader->setMat4("ModelMatrix", _model_matrix);
	shader->setVec3("material.diffuse", _water_material->diffuse);
	shader->setVec3("material.specular", _water_material->specular);
	shader->setFloat("material.shininess", _water_material->shininess);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _main_water_text);
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, _foam_text);
	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_2D, _perlin_noise_text);
	glActiveTexture(GL_TEXTURE0 + 3);
	glBindTexture(GL_TEXTURE_2D, _terrain_perlin_noise_text);
	glActiveTexture(GL_TEXTURE0 + 4);
	glBindTexture(GL_TEXTURE_CUBE_MAP, _skybox);

	GLuint program = shader->getProgram();
	glUseProgram(program);
	glBindVertexArray(VAO);

	if (_wireframe)
	{
		glDrawElements(GL_LINES, _indices.size(), GL_UNSIGNED_INT, 0);
	}
	else
	{
		glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);
	}

	glBindVertexArray(0);
	glUseProgram(0);
}

void Water::updateInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
	{
		if (!_one_pressed)
		{
			_wireframe = !_wireframe;

		}
		_one_pressed = true;
	}
	else
	{
		_one_pressed = false;
	}
}

float Water::lerp(float a0, float a1, float w) {
	return (1.0f - w) * a0 + w * a1;
}

Water::~Water()
{
	delete[] _noise_seed_2d;
}