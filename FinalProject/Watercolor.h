#pragma once
#include "precompiled.h"
#include <GL/freeglut.h>
#include <glm/glm.hpp>

class ShaderProgram;
struct WatercolorInfo
{
	ShaderProgram *abstractionShader;
	ShaderProgram *watercolorShader;
	GLuint quadVAO;
	GLuint frameBuffer;
	GLuint textureColorBuffer;
	GLuint pigmentTexture, turbulenceTexture, paperTexture;
	float pixelWidth, pixelHeight;

	float brushThreshold;
	glm::vec3 brushColor;
	float kernelOffsetX, kernelOffsetY;

	float scaleFactorPigment;
	float scaleFactorTurbulence;
	float scaleFactorPaper;

	bool enableBrush, enableWobble, enableEdgeDarkening, enablePigment, enableTurbulence, enablePaper;
};