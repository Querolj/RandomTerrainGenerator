#pragma once

#include "libs.h";

class TextureManager
{
private:
	TextureManager();
public:
	static int loadTexture(GLuint& texture0, int img_width, int img_height, const char* img_path);
	static int loadTexture(GLuint& texture0, const char* img_path);
	static void setupFloatTexture(GLuint& texture0, float* data, int width, int height);
	static unsigned int loadCubemap(vector<std::string> faces);
};

int TextureManager::loadTexture(GLuint& texture0, int img_width, int img_height, const char* img_path)
{
	//Texture init

	unsigned char* image = SOIL_load_image(img_path, &img_width, &img_height,
		NULL, SOIL_LOAD_RGBA);

	glGenTextures(1, &texture0);
	glBindTexture(GL_TEXTURE_2D, texture0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


	if (image)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_width, img_height, 0,
			GL_RGBA, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		cout << "ERROR setTexture : Could not load texture " << image << endl;
		return -1;
	}

	glActiveTexture(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	SOIL_free_image_data(image);

	return 0;
}


int TextureManager::loadTexture(GLuint& texture, const char* img_path)
{
	//Texture init
	int img_width, img_height;
	unsigned char* image = SOIL_load_image(img_path, &img_width, &img_height,
		NULL, SOIL_LOAD_RGBA);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


	if (image)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_width, img_height, 0,
			GL_RGBA, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		cout << "ERROR setTexture : Could not load texture " << image << endl;
		return -1;
	}

	glActiveTexture(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	SOIL_free_image_data(image);

	return 0;
}

void TextureManager::setupFloatTexture(GLuint& texture, float* data, int width, int height)
{
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0,
		GL_RED, GL_FLOAT, data);

	glActiveTexture(0);
	glBindTexture(GL_TEXTURE_2D, 0);

}

unsigned int TextureManager::loadCubemap(vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = SOIL_load_image(faces[i].c_str(), &width, &height,
			NULL, SOIL_LOAD_RGB);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}