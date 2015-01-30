#pragma once
//#include "precompiled.h"
#include <GL/glew.h>
#include <unordered_set>

#include <luapath/luapath.hpp>
#include "ShaderProgram.h"

enum  ShaderType{GOURAUD, PHONG, CEL};

/**@brief A singleton class that manages the lifetime of ShaderProgam objects. 
@details Each pair(vertex shader, fragment shader) is considered a unique ShaderProgram.Loading of shaders should be done 
through this class. If a requested pair(vertex shader, fragment shader) has been loaded then the existing cached 
program is returned so duplication does not occur. A ShaderProgram is associated at Model level but different models
can use the same ShaderProgram.
*/
class ShaderManager
{
	
public:
	static ShaderManager& get();
	/**delete list of ShaderProgram */
	~ShaderManager();

	/**Load the shaders and return a pointer to the resulting program. May already be cached*/
	ShaderProgram* getShaderProgram(const std::string &vertexShader, const std::string &fragmentShader, ShaderType shaderType);

private:
	ShaderManager();
	GLuint addShader(const char* shaderSource, GLenum shaderType);
	char* getFileContents(const char* filePath);
	/**After the program has been loaded -- deletes the individual shader objects that were used to build program */
	GLuint createProgram(const char* vertexShaderSource, const char* fragmentShaderSource);
	void queryVariables(ShaderProgram *shaderProgram);

	void loadGouraudShader(ShaderProgram *shaderProgram);
	void loadPhongShader(ShaderProgram *shaderProgram);
	void loadCelShader(ShaderProgram *shaderProgram);
	GLuint generateUniformBlock(ShaderProgram *shaderProgram, GLuint bindingPoint, const std::string &blockName, const std::vector<GLfloat> &initialData);


private:
	typedef std::unordered_set<ShaderProgram*> shaderProgramSet;
	shaderProgramSet loadedShaderPrograms;

	GLuint currProgramId; //returned from createProgram
	//it is good practice to delete the shader after they have been linked
	GLuint tempVertexShaderId;
	GLuint tempFragmentShaderId;
	
};