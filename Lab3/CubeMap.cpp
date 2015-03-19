#include "CubeMap.h"
#include "Model.h"
#include "ShaderProgram.h"
#include <SOIL.h>
#include <GL/glew.h>

using std::string;

CubeMap::CubeMap()
{

}

CubeMap::CubeMap(float scaleFactor, const std::string &modelFilePath, ShaderProgram *shader, const std::string &baseFileName, const std::string &fileDirectory, const std::string &fileExtension)
{
	create(scaleFactor, modelFilePath, shader, baseFileName, fileDirectory, fileExtension);
}

void CubeMap::create(float scaleFactor, const std::string &modelFilePath, ShaderProgram *shader, const std::string &baseFileName, const std::string &fileDirectory, const std::string &fileExtension)
{
	m_Model = new Model("cubemap", modelFilePath, shader);
	m_ShaderProgram = shader;
	m_Scale = scaleFactor;

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &m_TexID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_TexID);
	const char * suffixes[] = { "posx", "negx", "posy",	"negy", "posz", "negz" };
	GLuint targets[] = {
		GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
	};

	for( int i = 0; i < 6; i++ )
	{
		string filepath = fileDirectory + baseFileName + suffixes[i] + fileExtension;
		int width, height;
		unsigned char* image = SOIL_load_image(filepath.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
		if (!image)
		{
			std::cout << "Could not load image : " << filepath << std::endl;
		}
		else
		{
			glTexImage2D(targets[i], 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
			SOIL_free_image_data(image);
		}
	}
	// Typical cube map settings
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER,	GL_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S,	GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T,	GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R,	GL_CLAMP_TO_EDGE);

}

CubeMap::~CubeMap()
{
}

void CubeMap::writeCubeMapTexture(ShaderProgram *shader, const std::string &variableName, int texUnit) const
{	
	glActiveTexture(GL_TEXTURE0 + texUnit);
	glUniform1i(shader->getVariableLocation(variableName), texUnit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_TexID);
	glActiveTexture(GL_TEXTURE0);
}