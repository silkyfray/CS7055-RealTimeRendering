#include "Utilities.h"

using namespace std;
Utilities::Utilities()
{
}


Utilities::~Utilities()
{
}

GLint Utilities::loadTextureFromFile(const string &filePath, bool loadColor, bool generateMipmap, GLuint textureWrap, GLuint textureMinFilter, GLuint textureMagFilter)
{
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	int width, height, channels;
	unsigned char* image;
	if (loadColor)
	{
		image = SOIL_load_image(filePath.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

	}
	else
	{
		image = SOIL_load_image(filePath.c_str(), &width, &height, &channels, SOIL_LOAD_L);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, image);
	}
	if (!image)
		std::cout << "Could not load image : " << filePath;

	if (generateMipmap)
		glGenerateMipmap(generateMipmap);
	// Parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textureWrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textureWrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, textureMinFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, textureMagFilter);
	glBindTexture(GL_TEXTURE_2D, 0);
	SOIL_free_image_data(image);
	return textureID;
}
