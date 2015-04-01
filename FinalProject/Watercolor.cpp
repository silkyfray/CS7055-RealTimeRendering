#include "Watercolor.h"
#include "ShaderProgram.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void WatercolorInfo::render() const
{
	/////////////////////////////////////////////////////
	// Bind to default framebuffer again and draw the 
	// quad plane with attched screen texture.
	// //////////////////////////////////////////////////
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// Clear all relevant buffers
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // Set clear color to white (not really necessery actually, since we won't be able to see behind the quad anyways)
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST); // We don't care about depth information when rendering a single quad

	// Draw Screen
	glUseProgram(watercolorShader->m_Id);
	glBindVertexArray(quadVAO);

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(watercolorShader->getVariableLocation("screenTexture"), 0);
	glBindTexture(GL_TEXTURE_2D, textureColorBuffer);	// Use the color attachment texture as the texture of the quad plane
	glActiveTexture(GL_TEXTURE0 + 1);
	glUniform1i(watercolorShader->getVariableLocation("pigmentTexture"), 1);
	glBindTexture(GL_TEXTURE_2D, pigmentTexture);
	glActiveTexture(GL_TEXTURE0 + 2);
	glUniform1i(watercolorShader->getVariableLocation("turbulenceTexture"), 2);
	glBindTexture(GL_TEXTURE_2D, turbulenceTexture);
	glActiveTexture(GL_TEXTURE0 + 3);
	glUniform1i(watercolorShader->getVariableLocation("paperTexture"), 3);
	glBindTexture(GL_TEXTURE_2D, paperTexture);

	glUniform1fv(watercolorShader->getVariableLocation("pixelWidth"), 1, &pixelWidth);
	glUniform1fv(watercolorShader->getVariableLocation("pixelHeight"), 1, &pixelHeight);

	glUniform1fv(watercolorShader->getVariableLocation("offsetX"), 1, &kernelOffsetX);
	glUniform1fv(watercolorShader->getVariableLocation("offsetY"), 1, &kernelOffsetY);

	glUniform1fv(watercolorShader->getVariableLocation("brushThreshold"), 1, &brushThreshold);
	glUniform3fv(watercolorShader->getVariableLocation("brushColor"), 1, glm::value_ptr(brushColor));

	glUniform1fv(watercolorShader->getVariableLocation("scaleFactorPigment"), 1, &scaleFactorPigment);
	glUniform1fv(watercolorShader->getVariableLocation("scaleFactorTurbulence"), 1, &scaleFactorTurbulence);
	glUniform1fv(watercolorShader->getVariableLocation("scaleFactorPaper"), 1, &scaleFactorPaper);


	glUniform1i(watercolorShader->getVariableLocation("enableBrush"), (enableBrush ? 1 : 0));
	glUniform1i(watercolorShader->getVariableLocation("enableWobble"), (enableWobble ? 1 : 0));
	glUniform1i(watercolorShader->getVariableLocation("enableEdgeDarkening"), (enableEdgeDarkening ? 1 : 0));
	glUniform1i(watercolorShader->getVariableLocation("enablePigment"), (enablePigment ? 1 : 0));
	glUniform1i(watercolorShader->getVariableLocation("enableTurbulence"), (enableTurbulence ? 1 : 0));
	glUniform1i(watercolorShader->getVariableLocation("enablePaper"), (enablePaper ? 1 : 0));
	//uniform bool enableBrush, enableWobble, enableEdgeDarkening, enablePigment, enableTurbulence, enablePaper;


	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}