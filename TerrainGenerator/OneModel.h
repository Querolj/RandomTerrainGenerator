#pragma once
#include "Model.h";

class OneModel : public Model {
public:
	OneModel();
	OneModel(const char* path, shared_ptr<Material> _material);

private:
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
};

OneModel::OneModel() 
{
	_model_matrix = glm::mat4(1.f);
}
OneModel::OneModel(const char* path, shared_ptr<Material> material)
{
	//Instantiate matrix
	_model_matrix = glm::mat4(1.f);
	loadModel(path);
	_material = material;
}


Mesh OneModel::processMesh(aiMesh* mesh, const aiScene* scene)
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

	//Appel de fucntion, return all_vertices et all indices


	return Mesh(vertices, indices, textures);
}
