#pragma once
#include <SOIL.h>
#include <GL/glew.h>
#include "precompiled.h"

class Utilities
{
public:
	static GLint loadTextureFromFile(const std::string &filePath, bool loadColor, bool generateMipmap, GLuint textureWrap, GLuint textureMinFilter, GLuint textureMagFilter);
	static GLuint generateAttachmentTexture(GLboolean depth, GLboolean stencil, int width, int height);
private:
	Utilities();
	~Utilities();
};

