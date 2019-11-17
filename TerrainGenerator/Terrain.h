#pragma once
#include "VerticesStruct.h"
#include "GroupModel.h"
#include "Water.h";

#include <math.h>

class Terrain
{
public:

	glm::mat4 _model_matrix;
	//Textures
	GLuint _herb_text;
	GLuint _rock_text;
	GLuint _rock_text2;
	GLuint _dirt_text;
	GLuint _shadow_map;

	/*
		Terrain properties
	*/
	//Display or not some element
	bool _display_water= true;
	bool _display_grass = true;

	//Precision of the model
	float _mesh_precision;
	float _height_multiplier = 1.f;
	int _grass_multiplier = 1;

	//Perlin noise
	float* _perlin_noise;

	//Functions 
	Terrain(int n, float precision, shared_ptr<Material> terrain_material);
	~Terrain();
	void Draw(Shader* shader);
	void generateRandomTerrain();

	//use to check input, debug mode
	void updateInput(GLFWwindow* window);

	//Environnement
	void setGrass(Shader* shader, const char* model_path, shared_ptr<Material> material);

private:
	//Size of the grid
	int _grid_size;
	//vertex data, constains vertex position, text coord and normal
	vector<Vertex> _vertices;
	vector<unsigned int> _indices;

	// Render data
	unsigned int VAO, VBO, EBO;
	bool _wireframe = false;
	shared_ptr<Material> _terrain_material;

	//Random values
	glm::vec2* _noise_seed_2d = nullptr;

	//input variables
	bool _one_pressed = false;
	bool _n_pressed = false;
	bool _o_pressed = false;

	//Environnement properties

	//grass
	vector<glm::vec3> _grass_positions; //positions where to draw grass
	Shader* _grass_shader = nullptr;
	const char* _grass_path;
	shared_ptr<Material> _grass_mat;
	GroupModel _grass;

	//Set vao & co
	void setupTerrainMesh();
	
	//Perlin Noise
	void generateRandomValues();
	float calculatePerlinNoise(glm::vec2 position, float scale);
	float lerp(float a0, float a1, float w);
	glm::vec3 lerp(glm::vec3 v1, glm::vec3 v2, float l);
	
	//Setup environnement
	void setupEnvironnement(); //Register position for environnement models
	glm::vec3 choosePlaceInPlane(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 v4);
	void drawEnvironnement(Shader* shader); //Draw environnement
	//Calcul functions
	void setupAllNormals();
	glm::vec3 calculateNormal(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3);

};

Terrain::Terrain(int n, float precision, shared_ptr<Material> terrain_material)
{
	_model_matrix = glm::mat4(1.f);

	this->_terrain_material = terrain_material;
	this->_grid_size = n;
	this->_mesh_precision = precision;
}

void Terrain::generateRandomValues()
{
	int n_sq = _grid_size * _grid_size;
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

//Perlin
void Terrain::generateRandomTerrain()
{
	int seed_len = _grid_size * _grid_size + 2;
	int n_precision = floor(_grid_size * _mesh_precision);
	_perlin_noise = new float[(n_precision + 1) * (n_precision + 1)];

	generateRandomValues();

	_vertices.clear();
	_indices.clear();
	glm::vec3 default_normal(0.f, 1.f, 0.f);
	//Generate 
	int x, y, index = 0;
	for (x = 0; x <= n_precision; x++)
	{
		for (y = 0; y <= n_precision; y++)
		{
			float pos_x = (float)x / _mesh_precision;
			float pos_y = (float)y / _mesh_precision;

			float height = calculatePerlinNoise(glm::vec2(pos_x, pos_y), 0.02f);
			float bump = calculatePerlinNoise(glm::vec2(pos_x, pos_y), 0.25f);

			height *= _height_multiplier;
			height += bump;

			_perlin_noise[x * n_precision + y] = height;

			//Calculate normal vector 
			glm::vec3 vertex_position = glm::vec3(pos_x, height, pos_y);
			
			Vertex vertex = { vertex_position, glm::vec2(pos_x, pos_y), default_normal };
			_vertices.push_back(vertex);

			if (x == 0) //No indice set on the first line
			{
				index++;
				continue;
			}

			if (y != n_precision) // right triangle
			{
				_indices.push_back(index);
				_indices.push_back(index - n_precision - 1);
				_indices.push_back(index - n_precision);
			}

			if (y != 0) //left triangle
			{
				_indices.push_back(index);
				_indices.push_back(index - 1);
				_indices.push_back(index - n_precision - 1);
			}
			index++;
		}

	}

	setupAllNormals();
	setupTerrainMesh();
	if (_display_grass)
	{
		setupEnvironnement();
	}
}

float Terrain::calculatePerlinNoise(glm::vec2 position, float scale)
{
	int seed_len = _grid_size * _grid_size + 2;

	float ajusted_pos_x = position.x * scale;
	float ajusted_pos_y = position.y * scale;

	glm::vec2 vertex_position_2d = glm::vec2(ajusted_pos_x, ajusted_pos_y);
	//dot
	int x0 = floor(ajusted_pos_x);
	int y0 = floor(ajusted_pos_y);
	int x1 = ceil(ajusted_pos_x);
	int y1 = ceil(ajusted_pos_y);

	glm::vec2 dir1 = glm::vec2(x0, y0) - vertex_position_2d;
	glm::vec2 dir2 = glm::vec2(x1, y0) - vertex_position_2d;
	glm::vec2 dir3 = glm::vec2(x0, y1) - vertex_position_2d;
	glm::vec2 dir4 = glm::vec2(x1, y1) - vertex_position_2d;

	float dot1 = glm::dot(dir1, _noise_seed_2d[(x0 * _grid_size + y0) % seed_len]);
	float dot2 = glm::dot(dir2, _noise_seed_2d[(x1 * _grid_size + y0) % seed_len]);
	float dot3 = glm::dot(dir3, _noise_seed_2d[(x0 * _grid_size + y1) % seed_len]);
	float dot4 = glm::dot(dir4, _noise_seed_2d[(x1 * _grid_size + y1) % seed_len]);

	float sx = ajusted_pos_x - (float)x0;
	float sy = ajusted_pos_y - (float)y0;

	//interpolation
	float lerp1 = lerp(dot1, dot2, sx);
	float lerp2 = lerp(dot3, dot4, sx);

	float height = lerp(lerp1, lerp2, sy);

	return height;
}

void Terrain::setupEnvironnement()
{
	_grass_positions.clear();
	int index = 0;
	int n_precision = floor(_grid_size * _mesh_precision);
	GroupModel grass_group(_grass_mat);
	grass_group.scaleModel(0.4f);

	for (int x = 1; x < n_precision; x++)
	{
		for (int y = 1; y < n_precision; y++)
		{
			index++;
			/*if (index >= _vertices.size())
			{
				break;
			}*/

			glm::vec3 top_left = _vertices[index % _vertices.size()].position;

			

			glm::vec3 top_right = _vertices[(index - 1) % _vertices.size()].position;
			glm::vec3 down_left = _vertices[(index + _grid_size) % _vertices.size()].position;
			glm::vec3 down_right = _vertices[(index + _grid_size - 1) % _vertices.size()].position;

			for (int i = 0; i < _grass_multiplier; i++)
			{
				if (top_left.y < 1.f || top_left.y > 8.f)
				{
					continue;
				}

				glm::vec3 grass_position = choosePlaceInPlane(top_left, top_right, down_left, down_right);
				if (grass_position.y > 1.f && grass_position.y < 4.f && glm::distance(top_left, grass_position) < 2.f)
				{
					grass_position /= grass_group._scale_factor;
					grass_position.y -= 0.1f;
					_grass_positions.push_back(grass_position);
				}
			}
			
		}
	}

	cout << " grass  : " << _grass._meshes_positions.size() << endl;
	grass_group.setupModel(_grass_path, _grass_positions);

	_grass = grass_group;
}

glm::vec3 Terrain::choosePlaceInPlane(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 v4)
{
	float lerp1 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	float lerp2 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	float lerp3 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

	glm::vec3 lerped_vertex = lerp(v1, v2, lerp1);
	lerped_vertex = lerp(lerped_vertex, v3, lerp2);
	lerped_vertex = lerp(lerped_vertex, v4, lerp3);

	return lerped_vertex;
}

void Terrain::setupAllNormals()
{
	int line_lenght = floor(_grid_size * _mesh_precision);
	for (int i = 0; i <= line_lenght; i++)
	{
		for (int j = 0; j <= line_lenght; j++)
		{
			int index = i * line_lenght + j;
			if (index - line_lenght < 0)
			{
				break;
			}
			glm::vec3 normal = calculateNormal(_vertices[index].position,
				_vertices[(index - line_lenght + 1)].position,
				_vertices[(index - line_lenght)].position);
			

			_vertices[index - line_lenght + 1].normal = normal;
			_vertices[index - line_lenght].normal = normal;
			_vertices[index].normal = normal;

		}
	}
}
void Terrain::updateInput(GLFWwindow* window)
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

	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
	{
		if (!_n_pressed)
		{
			//generateRandomTerrain();
		}
		_n_pressed = true;
	}
	else
	{
		_n_pressed = false;
	}
}

glm::vec3 Terrain::calculateNormal(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3)
{
	glm::vec3 normal;
	glm::vec3 u = normalize(v2 - v1);
	glm::vec3 v = normalize(v3 - v1);

	normal.x = (u.y * v.z) - (u.z * v.y);
	normal.y = (u.z * v.x) - (u.x * v.z);
	normal.z = (u.x * v.y) - (u.y * v.x);

	return glm::normalize(normal);
}

void Terrain::setupTerrainMesh()
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

void Terrain::Draw(Shader* shader)
{
	shader->setMat4("ModelMatrix", _model_matrix);
	shader->setVec3("material.diffuse", _terrain_material->diffuse);
	shader->setVec3("material.specular", _terrain_material->specular);
	shader->setFloat("material.shininess", _terrain_material->shininess);
	shader->setFloat("HeightMultiplier", _height_multiplier);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _herb_text);
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, _rock_text);
	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_2D, _rock_text2);
	glActiveTexture(GL_TEXTURE0 + 3);
	glBindTexture(GL_TEXTURE_2D, _shadow_map);

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

	drawEnvironnement(shader);
}

void Terrain::drawEnvironnement(Shader* shader)
{
	int i = 0;
	
	//Draw grass
	if (_display_grass)
	{
		glDisable(GL_CULL_FACE);
		_grass.Draw(_grass_shader);
		glEnable(GL_CULL_FACE);
	}
}


void Terrain::setGrass(Shader* shader, const char* model_path, shared_ptr<Material> material)
{
	this->_grass_shader = shader;
	this->_grass_path = model_path;
	this->_grass_mat = material;
}

float Terrain::lerp(float a0, float a1, float w) {
	return (1.0f - w) * a0 + w * a1;
}

glm::vec3 Terrain::lerp(glm::vec3 v1, glm::vec3 v2, float l)
{
	glm::vec3 lerped_vertex(0.f);
	lerped_vertex.x = lerp(v1.x, v2.x, l);
	lerped_vertex.y = lerp(v1.y, v2.y, l);
	lerped_vertex.z = lerp(v1.z, v2.z, l);

	return lerped_vertex;
}
Terrain::~Terrain()
{
	delete[] _noise_seed_2d;
	delete _grass_shader;
}