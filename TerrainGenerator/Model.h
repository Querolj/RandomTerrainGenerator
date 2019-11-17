#pragma once


#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "Shader.h"
#include "Material.h"

vector<Texture> textures_loaded; //every loaded textures	

class Model
{
public:

	//Draw the model with a material
	void Draw(Shader* shader);

	//translation, rotation & scaling
	void setPosition(glm::vec3 position);
	void translateModel(glm::vec3 translation);
	void rotateModel(float angle, glm::vec3 rotation);
	void scaleModel(glm::vec3 scaling);
	void scaleModel(float scaling);

	void replaceMatrix(glm::mat4 mat);
	//Material used by the model
	shared_ptr<Material> _material;

	float _scale_factor = 1.f;
protected:
	/*  Model Data  */
	vector<Mesh> _meshes;
	string _directory;

	//Load model
	void loadModel(string path);
	void processNode(aiNode* node, const aiScene* scene);
	virtual Mesh processMesh(aiMesh* mesh, const aiScene* scene) = 0;
	vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type,
		string typeName);

	//model matrix
	glm::mat4 _model_matrix;
};

void Model::Draw(Shader* shader)
{
	shader->setMat4("ModelMatrix", _model_matrix);
	shader->setVec3("material.diffuse", _material->diffuse);
	shader->setVec3("material.specular", _material->specular);
	shader->setFloat("material.shininess", _material->shininess);
	//set view & projection matrix
	for (unsigned int i = 0; i < _meshes.size(); i++)
	{
	
		_meshes[i].Draw(shader);
	}
}

inline void Model::translateModel(glm::vec3 translation)
{
	_model_matrix = glm::translate(_model_matrix, translation);
}

inline void Model::rotateModel(float angle, glm::vec3 rotation)
{
	_model_matrix = glm::rotate(_model_matrix, glm::radians(angle), rotation);

}

inline void Model::scaleModel(glm::vec3 scaling)
{
	_model_matrix= glm::scale(_model_matrix, scaling);
}

void Model::scaleModel(float scaling)
{
	_scale_factor *= scaling;
	scaleModel(glm::vec3(scaling));
}

inline void Model::setPosition(glm::vec3 position)
{
	translateModel(glm::vec3(0.f, 0.f, 0.f));
	translateModel(position);
}

void Model::replaceMatrix(glm::mat4 mat)
{
	_model_matrix = mat;
}

void Model::loadModel(string path)
{
	
	//Load 3D model
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		cout << "ERROR::ASSIMP::" << import.GetErrorString() << endl;
		return;
	}
	_directory = path.substr(0, path.find_last_of('/'));

	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	// process all the node's meshes (if any)
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		_meshes.push_back(processMesh(mesh, scene));
	}
	// then do the same for each of its children
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
{
	vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		bool skip = false;
		for (unsigned int j = 0; j < textures_loaded.size(); j++)
		{
			if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
			{
				textures.push_back(textures_loaded[j]);
				skip = true;
				const char* _str = str.C_Str();
				string filename = string(_str);
				cout << "SKIPPING : "<< filename << endl;
				break;
			}
		}
		if (!skip)
		{   // if texture hasn't been loaded already, load it
			Texture texture;
			const char* _str = str.C_Str();
			string filename = string(_str);
			string path = _directory + '/' + filename;
			cout << path << endl;
			TextureManager::loadTexture(texture.id, path.c_str());

			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			textures_loaded.push_back(texture); // add to loaded textures
		}
	}
	return textures;
}

