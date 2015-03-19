#include "precompiled.h"
#include "ShaderManager.h"
#include <GL/glew.h>

using std::endl;
using std::cout;
using std::string;
using std::vector;
ShaderManager& ShaderManager::get()
{
	static ShaderManager singleton;
	return singleton;
}

ShaderManager::ShaderManager()
{

}

ShaderManager::~ShaderManager()
{
	shaderProgramSet::iterator it = loadedShaderPrograms.begin();
	for (; it != loadedShaderPrograms.end(); ++it)
		delete *it;
}

void ShaderManager::queryVariables(ShaderProgram *shaderProgram)
{
	GLint numActiveAttribs = 0;
	GLint numActiveUniforms = 0;
	GLint numActiveUniformBlocks = 0;
	glGetProgramInterfaceiv(shaderProgram->m_Id, GL_PROGRAM_INPUT, GL_ACTIVE_RESOURCES, &numActiveAttribs);
	glGetProgramInterfaceiv(shaderProgram->m_Id, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numActiveAttribs);
	glGetProgramInterfaceiv(shaderProgram->m_Id, GL_UNIFORM_BLOCK, GL_ACTIVE_RESOURCES, &numActiveAttribs);
	cout << "-------Shader properties for: (" << shaderProgram->m_ShaderNames.vertexShader << ","
		 << shaderProgram->m_ShaderNames.vertexShader << ")" << "-----------" << endl;
	cout << "number of active attributes: " << numActiveAttribs << endl;
	cout << "number of active uniforms: " << numActiveUniforms << endl;
	cout << "number of active uniforms blocks: " << numActiveUniformBlocks << endl;

	std::vector<GLchar> nameData(256);
	std::vector<GLenum> properties;
	properties.push_back(GL_NAME_LENGTH);
	properties.push_back(GL_TYPE);
	std::vector<GLint> values(properties.size());
	for(int attrib = 0; attrib < numActiveAttribs; ++attrib)
	{
	  glGetProgramResourceiv(shaderProgram->m_Id, GL_PROGRAM_INPUT, attrib, properties.size(),
		&properties[0], values.size(), NULL, &values[0]);
	  nameData.resize(properties[0]); //The length of the name.
	  glGetProgramResourceName(shaderProgram->m_Id, GL_PROGRAM_INPUT, attrib, nameData.size(), NULL, &nameData[0]);
	  std::string name((char*)&nameData[0], nameData.size() - 1);

	  //cout << "name: " << name << " type: " << values[1] << endl;
	}
}

ShaderProgram* ShaderManager::getShaderProgram(const std::string &vertexShader, const std::string &fragmentShader, ShaderType shaderType)
{

	ShaderProgram *newShader = new ShaderProgram(ShaderProgram::Key(vertexShader, fragmentShader));
	shaderProgramSet::const_iterator it =
		std::find_if(loadedShaderPrograms.begin(), loadedShaderPrograms.end(),
		[newShader](const ShaderProgram* other) -> bool { return *newShader == *other; });

	if (it == loadedShaderPrograms.end())
	{
		GLuint programId = createProgram(vertexShader.c_str(), fragmentShader.c_str());
		newShader->m_Id = programId;

		//queryVariables(newShader);
		newShader->m_ShaderType = shaderType;
		loadShaderVariables(newShader);
		loadedShaderPrograms.insert(newShader);
		return newShader;
	}
	else
	{
		delete newShader;
		//return existing program with those names
		return *it;
	}
	
}


GLuint ShaderManager::createProgram(const char* vertexShaderSource, const char* fragmentShaderSource)
{
	currProgramId = glCreateProgram();
	if (currProgramId == 0) {
		std::cout << "Error creating shader program\n";
	}
	// Create two shader objects, one for the vertex, and one for the fragment shader
	tempVertexShaderId = addShader(vertexShaderSource, GL_VERTEX_SHADER);
	tempFragmentShaderId = addShader(fragmentShaderSource, GL_FRAGMENT_SHADER);

	GLint success = 0;
	GLchar errorLog[1024] = { 0 };

	// After compiling all shader objects and attaching them to the program, we can finally link it
	glLinkProgram(currProgramId);
	// program has been successfully linked but needs to be validated to check whether the program can execute given the current pipeline state
	glValidateProgram(currProgramId);
	// check for program related errors using glGetProgramiv
	glGetProgramiv(currProgramId, GL_VALIDATE_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(currProgramId, sizeof(errorLog), NULL, errorLog);
		std::cout << "Invalid shader program: " << errorLog << std::endl;
	}

	//cleanup
	glDetachShader(currProgramId, tempVertexShaderId);
	glDetachShader(currProgramId, tempFragmentShaderId);
	glDeleteShader(tempVertexShaderId);
	glDeleteShader(tempFragmentShaderId);

	return currProgramId;
}

GLuint ShaderManager::addShader(const char* shaderSource, GLenum shaderType)
{
	// create a shader object
	GLuint shaderId = glCreateShader(shaderType);

	if (shaderId == 0) {
		std::cout << "Error creating shader type " << shaderType << std::endl;
	}
	char* shaderText = getFileContents(shaderSource);
	if(!shaderText)
		std::cout << "error reading shader source: " << shaderSource << std::endl;
	// Bind the source code to the shader, this happens before compilation
	glShaderSource(shaderId, 1, (const GLchar**)&shaderText, NULL);
	free(shaderText);
	// compile the shader and check for errors
	glCompileShader(shaderId);	
	GLint success;
	// check for shader related errors using glGetShaderiv
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar infoLog[1024];
		glGetShaderInfoLog(shaderId, 1024, NULL, infoLog);
		if (shaderType == GL_VERTEX_SHADER)
			std::cout << "Error compiling vertex shader: " << infoLog << std::endl;
		else if(shaderType == GL_FRAGMENT_SHADER)
			std::cout << "Error compiling fragment shader: " << infoLog << std::endl;
	}
	// Attach the compiled shader object to the program object
	glAttachShader(currProgramId, shaderId);
	return shaderId;
}

char* ShaderManager::getFileContents(const char* filePath)
{
	FILE* fp;
	fopen_s(&fp,filePath, "rb");

	if (fp == NULL) { return NULL; }

	fseek(fp, 0L, SEEK_END);
	long size = ftell(fp);

	fseek(fp, 0L, SEEK_SET);
	char* buf = new char[size + 1];
	fread(buf, 1, size, fp);
	buf[size] = '\0';

	fclose(fp);

	return buf;
}

void ShaderManager::loadShaderVariables(ShaderProgram *shaderProgram)
{
	//shaderProgram->m_ActiveVariables["position"] = glGetAttribLocation(shaderProgram->m_Id, "position");
	//shaderProgram->m_ActiveVariables["normal"] = glGetAttribLocation(shaderProgram->m_Id, "normal");
	//shaderProgram->m_ActiveVariables["MVP"] = glGetUniformLocation(shaderProgram->m_Id, "m_pvm");
	//shaderProgram->m_ActiveVariables["viewModelMatrix"] = glGetUniformLocation(shaderProgram->m_Id, "m_viewModel");
	//shaderProgram->m_ActiveVariables["normalMatrix"] = glGetUniformLocation(shaderProgram->m_Id, "m_normal");
	//shaderProgram->m_ActiveVariables["l_dir"] = glGetUniformLocation(shaderProgram->m_Id, "l_dir");
	//shaderProgram->m_ActiveVariables["diffuse"] = glGetUniformLocation(shaderProgram->m_Id, "diffuse");
	//shaderProgram->m_ActiveVariables["ambient"] = glGetUniformLocation(shaderProgram->m_Id, "ambient");
	//shaderProgram->m_ActiveVariables["specular"] = glGetUniformLocation(shaderProgram->m_Id, "specular");
	//shaderProgram->m_ActiveVariables["shininess"] = glGetUniformLocation(shaderProgram->m_Id, "shininess");
	
	//for(auto const &var : shaderVariables)
	//{
	//	shaderProgram->m_ActiveVariables[var] = glGetAttribLocation(shaderProgram->m_Id, var.c_str());
	//}
	switch (shaderProgram->m_ShaderType)
	{
	case ShaderType::PHONG:
	case ShaderType::GOURAUD:
	case ShaderType::CEL:
		shaderProgram->m_ActiveVariables["position"] = glGetAttribLocation(shaderProgram->m_Id, "position");
		shaderProgram->m_ActiveVariables["normal"] = glGetAttribLocation(shaderProgram->m_Id, "normal");
		shaderProgram->m_ActiveVariables["MVP"] = glGetUniformLocation(shaderProgram->m_Id, "m_pvm");
		shaderProgram->m_ActiveVariables["viewModelMatrix"] = glGetUniformLocation(shaderProgram->m_Id, "m_viewModel");
		shaderProgram->m_ActiveVariables["normalMatrix"] = glGetUniformLocation(shaderProgram->m_Id, "m_normal");
		shaderProgram->m_ActiveVariables["lightDirection"] = glGetUniformLocation(shaderProgram->m_Id, "l_dir");
		shaderProgram->m_ActiveVariables["diffuse"] = glGetUniformLocation(shaderProgram->m_Id, "diffuse");
		shaderProgram->m_ActiveVariables["ambient"] = glGetUniformLocation(shaderProgram->m_Id, "ambient");
		break;
	case ShaderType::CEL_OUTLINE:
		shaderProgram->m_ActiveVariables["position"] = glGetAttribLocation(shaderProgram->m_Id, "position");
		shaderProgram->m_ActiveVariables["normal"] = glGetAttribLocation(shaderProgram->m_Id, "normal");
		shaderProgram->m_ActiveVariables["MVP"] = glGetUniformLocation(shaderProgram->m_Id, "m_pvm");
		shaderProgram->m_ActiveVariables["projectionMatrix"] = glGetUniformLocation(shaderProgram->m_Id, "m_proj");
		shaderProgram->m_ActiveVariables["normalMatrix"] = glGetUniformLocation(shaderProgram->m_Id, "m_normal");
		shaderProgram->m_ActiveVariables["lineColor"] = glGetUniformLocation(shaderProgram->m_Id, "lineColor");
		shaderProgram->m_ActiveVariables["lineTickness"] = glGetUniformLocation(shaderProgram->m_Id, "lineThickness");
		break;
	case ShaderType::BLINNPHONG:
		shaderProgram->m_ActiveVariables["position"] = glGetAttribLocation(shaderProgram->m_Id, "position");
		shaderProgram->m_ActiveVariables["normal"] = glGetAttribLocation(shaderProgram->m_Id, "normal");
		shaderProgram->m_ActiveVariables["MVP"] = glGetUniformLocation(shaderProgram->m_Id, "m_pvm");
		shaderProgram->m_ActiveVariables["viewModelMatrix"] = glGetUniformLocation(shaderProgram->m_Id, "m_viewModel");
		shaderProgram->m_ActiveVariables["normalMatrix"] = glGetUniformLocation(shaderProgram->m_Id, "m_normal");
		shaderProgram->m_ActiveVariables["lightDirection"] = glGetUniformLocation(shaderProgram->m_Id, "l_dir");
		shaderProgram->m_ActiveVariables["diffuse"] = glGetUniformLocation(shaderProgram->m_Id, "diffuse");
		shaderProgram->m_ActiveVariables["ambient"] = glGetUniformLocation(shaderProgram->m_Id, "ambient");
		shaderProgram->m_ActiveVariables["specular"] = glGetUniformLocation(shaderProgram->m_Id, "specular");
		shaderProgram->m_ActiveVariables["shininess"] = glGetUniformLocation(shaderProgram->m_Id, "shininess");
		break;
	case ShaderType::COOKTOR:
		shaderProgram->m_ActiveVariables["position"] = glGetAttribLocation(shaderProgram->m_Id, "position");
		shaderProgram->m_ActiveVariables["normal"] = glGetAttribLocation(shaderProgram->m_Id, "normal");
		shaderProgram->m_ActiveVariables["MVP"] = glGetUniformLocation(shaderProgram->m_Id, "m_pvm");
		shaderProgram->m_ActiveVariables["viewModelMatrix"] = glGetUniformLocation(shaderProgram->m_Id, "m_viewModel");
		shaderProgram->m_ActiveVariables["normalMatrix"] = glGetUniformLocation(shaderProgram->m_Id, "m_normal");
		shaderProgram->m_ActiveVariables["lightDirection"] = glGetUniformLocation(shaderProgram->m_Id, "l_dir");
		shaderProgram->m_ActiveVariables["diffuse"] = glGetUniformLocation(shaderProgram->m_Id, "diffuse");
		shaderProgram->m_ActiveVariables["ambient"] = glGetUniformLocation(shaderProgram->m_Id, "ambient");
		shaderProgram->m_ActiveVariables["specular"] = glGetUniformLocation(shaderProgram->m_Id, "specular");
		shaderProgram->m_ActiveVariables["roughness"] = glGetUniformLocation(shaderProgram->m_Id, "roughness");
		shaderProgram->m_ActiveVariables["refraction"] = glGetUniformLocation(shaderProgram->m_Id, "refraction");
		break;
	case ShaderType::BLINNPHONG_TEXTURE:
		shaderProgram->m_ActiveVariables["position"] = glGetAttribLocation(shaderProgram->m_Id, "position");
		shaderProgram->m_ActiveVariables["normal"] = glGetAttribLocation(shaderProgram->m_Id, "normal");
		shaderProgram->m_ActiveVariables["texCoord"] = glGetAttribLocation(shaderProgram->m_Id, "texCoord");
		shaderProgram->m_ActiveVariables["MVP"] = glGetUniformLocation(shaderProgram->m_Id, "m_pvm");
		shaderProgram->m_ActiveVariables["viewModelMatrix"] = glGetUniformLocation(shaderProgram->m_Id, "m_viewModel");
		shaderProgram->m_ActiveVariables["normalMatrix"] = glGetUniformLocation(shaderProgram->m_Id, "m_normal");
		shaderProgram->m_ActiveVariables["lightDirection"] = glGetUniformLocation(shaderProgram->m_Id, "l_dir");
		shaderProgram->m_ActiveVariables["texture_diffuse1"] = glGetUniformLocation(shaderProgram->m_Id, "texture_diffuse1");
		shaderProgram->m_ActiveVariables["ambient"] = glGetUniformLocation(shaderProgram->m_Id, "ambient");
		shaderProgram->m_ActiveVariables["specular"] = glGetUniformLocation(shaderProgram->m_Id, "specular");
		shaderProgram->m_ActiveVariables["shininess"] = glGetUniformLocation(shaderProgram->m_Id, "shininess");
		break;
	case ShaderType::REFLECTREFRACT:
		shaderProgram->m_ActiveVariables["position"] = glGetAttribLocation(shaderProgram->m_Id, "position");
		shaderProgram->m_ActiveVariables["normal"] = glGetAttribLocation(shaderProgram->m_Id, "normal");
		shaderProgram->m_ActiveVariables["Material.Eta"] = glGetUniformLocation(shaderProgram->m_Id, "Material.Eta");
		shaderProgram->m_ActiveVariables["Material.ReflectionFactor"] = glGetUniformLocation(shaderProgram->m_Id, "Material.ReflectionFactor");
		shaderProgram->m_ActiveVariables["DrawSkyBox"] = glGetUniformLocation(shaderProgram->m_Id, "DrawSkyBox");
		shaderProgram->m_ActiveVariables["WorldCameraPosition"] = glGetUniformLocation(shaderProgram->m_Id, "WorldCameraPosition");
		shaderProgram->m_ActiveVariables["ModelViewMatrix"] = glGetUniformLocation(shaderProgram->m_Id, "ModelViewMatrix");
		shaderProgram->m_ActiveVariables["ModelMatrix"] = glGetUniformLocation(shaderProgram->m_Id, "ModelMatrix");
		shaderProgram->m_ActiveVariables["NormalMatrix"] = glGetUniformLocation(shaderProgram->m_Id, "NormalMatrix");
		shaderProgram->m_ActiveVariables["ProjectionMatrix"] = glGetUniformLocation(shaderProgram->m_Id, "ProjectionMatrix");
		shaderProgram->m_ActiveVariables["MVP"] = glGetUniformLocation(shaderProgram->m_Id, "MVP");
		shaderProgram->m_ActiveVariables["CubeMapTex"] = glGetUniformLocation(shaderProgram->m_Id, "CubeMapTex");
		break;
	case ShaderType::REFLECTREFRACT2:
		shaderProgram->m_ActiveVariables["position"] = glGetAttribLocation(shaderProgram->m_Id, "position");
		shaderProgram->m_ActiveVariables["normal"] = glGetAttribLocation(shaderProgram->m_Id, "normal");
		shaderProgram->m_ActiveVariables["Eta"] = glGetUniformLocation(shaderProgram->m_Id, "Eta");
		shaderProgram->m_ActiveVariables["FresnelPower"] = glGetUniformLocation(shaderProgram->m_Id, "FresnelPower");
		shaderProgram->m_ActiveVariables["F"] = glGetUniformLocation(shaderProgram->m_Id, "F");
		//shaderProgram->m_ActiveVariables["DrawSkyBox"] = glGetUniformLocation(shaderProgram->m_Id, "DrawSkyBox");
		shaderProgram->m_ActiveVariables["ModelViewMatrix"] = glGetUniformLocation(shaderProgram->m_Id, "ModelViewMatrix");
		shaderProgram->m_ActiveVariables["NormalMatrix"] = glGetUniformLocation(shaderProgram->m_Id, "NormalMatrix");
		shaderProgram->m_ActiveVariables["MVP"] = glGetUniformLocation(shaderProgram->m_Id, "MVP");
		shaderProgram->m_ActiveVariables["Cubemap"] = glGetUniformLocation(shaderProgram->m_Id, "Cubemap");
		break;
	case ShaderType::TEXTURECUBE:
		shaderProgram->m_ActiveVariables["MVP"] = glGetUniformLocation(shaderProgram->m_Id, "MVP");
		shaderProgram->m_ActiveVariables["Cubemap"] = glGetUniformLocation(shaderProgram->m_Id, "Cubemap");
		break;
	case ShaderType::NORMALMAP:
		shaderProgram->m_ActiveVariables["position"] = glGetAttribLocation(shaderProgram->m_Id, "position");
		shaderProgram->m_ActiveVariables["normal"] = glGetAttribLocation(shaderProgram->m_Id, "normal");
		shaderProgram->m_ActiveVariables["texCoord"] = glGetAttribLocation(shaderProgram->m_Id, "texCoord");
		shaderProgram->m_ActiveVariables["tangent"] = glGetAttribLocation(shaderProgram->m_Id, "tangent");
		shaderProgram->m_ActiveVariables["bitangent"] = glGetAttribLocation(shaderProgram->m_Id, "bitangent");
		shaderProgram->m_ActiveVariables["Light.Direction"] = glGetUniformLocation(shaderProgram->m_Id, "Light.Direction");
		shaderProgram->m_ActiveVariables["Light.Intensity"] = glGetUniformLocation(shaderProgram->m_Id, "Light.Intensity");
		shaderProgram->m_ActiveVariables["ModelViewMatrix"] = glGetUniformLocation(shaderProgram->m_Id, "ModelViewMatrix");
		shaderProgram->m_ActiveVariables["NormalMatrix"] = glGetUniformLocation(shaderProgram->m_Id, "NormalMatrix");
		shaderProgram->m_ActiveVariables["ProjectionMatrix"] = glGetUniformLocation(shaderProgram->m_Id, "ProjectionMatrix");
		shaderProgram->m_ActiveVariables["MVP"] = glGetUniformLocation(shaderProgram->m_Id, "MVP");
		shaderProgram->m_ActiveVariables["Material.Ka"] = glGetUniformLocation(shaderProgram->m_Id, "Material.Ka");
		shaderProgram->m_ActiveVariables["Material.Ks"] = glGetUniformLocation(shaderProgram->m_Id, "Material.Ks");
		shaderProgram->m_ActiveVariables["Material.Shininess"] = glGetUniformLocation(shaderProgram->m_Id, "Material.Shininess");
		shaderProgram->m_ActiveVariables["ColorTex"] = glGetUniformLocation(shaderProgram->m_Id, "ColorTex");
		shaderProgram->m_ActiveVariables["NormalMapTex"] = glGetUniformLocation(shaderProgram->m_Id, "NormalMapTex");
		shaderProgram->m_ActiveVariables["LightMapTex"] = glGetUniformLocation(shaderProgram->m_Id, "LightMapTex");
		shaderProgram->m_ActiveVariables["renderNormalMap"] = glGetUniformLocation(shaderProgram->m_Id, "renderNormalMap");
		shaderProgram->m_ActiveVariables["renderDiffuse"] = glGetUniformLocation(shaderProgram->m_Id, "renderDiffuse");
		break;
	case REFLECTREFRACTNORMALMAP:
		shaderProgram->m_ActiveVariables["position"] = glGetAttribLocation(shaderProgram->m_Id, "position");
		shaderProgram->m_ActiveVariables["normal"] = glGetAttribLocation(shaderProgram->m_Id, "normal");
		shaderProgram->m_ActiveVariables["Eta"] = glGetUniformLocation(shaderProgram->m_Id, "Eta");
		shaderProgram->m_ActiveVariables["FresnelPower"] = glGetUniformLocation(shaderProgram->m_Id, "FresnelPower");
		shaderProgram->m_ActiveVariables["F"] = glGetUniformLocation(shaderProgram->m_Id, "F");
		//shaderProgram->m_ActiveVariables["DrawSkyBox"] = glGetUniformLocation(shaderProgram->m_Id, "DrawSkyBox");
		shaderProgram->m_ActiveVariables["ModelViewMatrix"] = glGetUniformLocation(shaderProgram->m_Id, "ModelViewMatrix");
		shaderProgram->m_ActiveVariables["NormalMatrix"] = glGetUniformLocation(shaderProgram->m_Id, "NormalMatrix");
		shaderProgram->m_ActiveVariables["MVP"] = glGetUniformLocation(shaderProgram->m_Id, "MVP");
		shaderProgram->m_ActiveVariables["Cubemap"] = glGetUniformLocation(shaderProgram->m_Id, "Cubemap");

		shaderProgram->m_ActiveVariables["texCoord"] = glGetAttribLocation(shaderProgram->m_Id, "texCoord");
		shaderProgram->m_ActiveVariables["tangent"] = glGetAttribLocation(shaderProgram->m_Id, "tangent");
		shaderProgram->m_ActiveVariables["bitangent"] = glGetAttribLocation(shaderProgram->m_Id, "bitangent");
		shaderProgram->m_ActiveVariables["Light.Direction"] = glGetUniformLocation(shaderProgram->m_Id, "Light.Direction");
		shaderProgram->m_ActiveVariables["Light.Intensity"] = glGetUniformLocation(shaderProgram->m_Id, "Light.Intensity");
		shaderProgram->m_ActiveVariables["ProjectionMatrix"] = glGetUniformLocation(shaderProgram->m_Id, "ProjectionMatrix");
		shaderProgram->m_ActiveVariables["Material.Ka"] = glGetUniformLocation(shaderProgram->m_Id, "Material.Ka");
		shaderProgram->m_ActiveVariables["Material.Ks"] = glGetUniformLocation(shaderProgram->m_Id, "Material.Ks");
		shaderProgram->m_ActiveVariables["Material.Shininess"] = glGetUniformLocation(shaderProgram->m_Id, "Material.Shininess");
		shaderProgram->m_ActiveVariables["ColorTex"] = glGetUniformLocation(shaderProgram->m_Id, "ColorTex");
		shaderProgram->m_ActiveVariables["NormalMapTex"] = glGetUniformLocation(shaderProgram->m_Id, "NormalMapTex");
		shaderProgram->m_ActiveVariables["LightMapTex"] = glGetUniformLocation(shaderProgram->m_Id, "LightMapTex");
		shaderProgram->m_ActiveVariables["mixRRfactor"] = glGetUniformLocation(shaderProgram->m_Id, "mixRRfactor");
		break;
	}
};

//void ShaderManager::loadPhongShader(ShaderProgram *shaderProgram)
//{
//	loadGouraudShader(shaderProgram);
//}
//
//void ShaderManager::loadCelShader(ShaderProgram *shaderProgram)
//{
//	loadGouraudShader(shaderProgram);
//}

GLuint ShaderManager::generateUniformBlock(ShaderProgram *shaderProgram, GLuint bindingPoint, const std::string &blockName, const std::vector<GLfloat> &initialData)
{
	GLuint buffer, blockIndex;
	blockIndex = glGetUniformBlockIndex(shaderProgram->m_Id, blockName.c_str());
	glUniformBlockBinding(shaderProgram->m_Id, blockIndex, bindingPoint);
	
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_UNIFORM_BUFFER, buffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(initialData), &initialData[0], GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, buffer);
	return buffer;
}
