#pragma once
#include "precompiled.h"
#include <GL/glew.h>

class ShaderProgram;
class Model;

class CubeMap
{
public:
	CubeMap();
	CubeMap(float scaleFactor, const std::string &modelFilePath, ShaderProgram *shader, const std::string &baseFileName, const std::string &fileDirectory, const std::string &fileExtension);
	~CubeMap();
	void writeCubeMapTexture(ShaderProgram *shader, const std::string &variableName, int texUnit) const;
	void create(float scaleFactor, const std::string &modelFilePath, ShaderProgram *shader, const std::string &baseFileName, const std::string &fileDirectory, const std::string &fileExtension);

	GLuint m_TexID;
	ShaderProgram *m_ShaderProgram;
	Model *m_Model;
	float m_Scale;
};

