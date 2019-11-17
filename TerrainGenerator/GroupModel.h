#pragma once
#include "Model.h"

class GroupModel : public Model
{
public :
	GroupModel();
	GroupModel(shared_ptr<Material> _material);
	void setupModel(const char* path, vector<glm::vec3> meshes_positions);
	vector<glm::vec3> _meshes_positions;

private:
	vector<Vertex> _all_vertices;
	vector<unsigned int> _all_indices;

	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	void setupAllMeshes(vector<Vertex> vertices, vector<unsigned int> indices);
};

GroupModel::GroupModel() {}

GroupModel::GroupModel(shared_ptr<Material> material){
	//Instantiate matrix
	_model_matrix = glm::mat4(1.f);
	_material = material;
}

void GroupModel::setupModel(const char* path, vector<glm::vec3> meshes_positions)
{
	_meshes_positions = meshes_positions;

	loadModel(path);
}

Mesh GroupModel::processMesh(aiMesh* mesh, const aiScene* scene)
{
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		// process vertex positions, normals and texture coordinates
		glm::vec3 vector;
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.position = vector;

		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.normal = vector;

		if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
		{
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.texcoord = vec;
		}
		else
		{
			vertex.texcoord = glm::vec2(0.0f, 0.0f);
		}

		vertices.push_back(vertex);
	}
	// process indices

	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		vector<Texture> diffuseMaps = loadMaterialTextures(material,
			aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		vector<Texture> specularMaps = loadMaterialTextures(material,
			aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	}

	setupAllMeshes(vertices, indices);
	return Mesh(_all_vertices, _all_indices, textures);
}

void GroupModel::setupAllMeshes(vector<Vertex> vertices, vector<unsigned int> indices)
{
	int index_indice = 0;
	_all_vertices.clear();
	_all_indices.clear();
	//Calcul size offset 
	
	//Build vertices
	for (vector<glm::vec3>::iterator mesh_pos = _meshes_positions.begin(); mesh_pos != _meshes_positions.end(); ++mesh_pos)
	{
		vector<Vertex> offset_vertices;
		vector<int> offset_indices;

		//Random rotation
		float random_rotation = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		rotateModel(180 * random_rotation, glm::vec3(0.f, 0.f, 1.f));

		for (vector<Vertex>::iterator current_vertex = vertices.begin(); current_vertex != vertices.end(); ++current_vertex)
		{
			Vertex vertex = (*current_vertex);
			vertex.position += (*mesh_pos);
			
			offset_vertices.push_back(vertex);
		}

		for (vector<unsigned int>::iterator current_indice= indices.begin(); current_indice != indices.end(); ++current_indice)
		{
			unsigned int offset_indice = (*current_indice) + index_indice;
			offset_indices.push_back(offset_indice);
		}

		index_indice += vertices.size(); 
		_all_vertices.insert(_all_vertices.end(), offset_vertices.begin(), offset_vertices.end());
		_all_indices.insert(_all_indices.end(), offset_indices.begin(), offset_indices.end());

		rotateModel(-180 * random_rotation, glm::vec3(0.f, 0.f, 1.f));

	}
	cout << "setup all "<< " : "<< _meshes_positions.size() <<endl;
}