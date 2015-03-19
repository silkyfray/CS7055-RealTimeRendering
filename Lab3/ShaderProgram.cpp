#include "ShaderProgram.h"
#include <exception>

ShaderProgram::Key::Key(const std::string vertexShader, const std::string fragmentShader)
	:vertexShader(vertexShader), fragmentShader(fragmentShader)
{

}
bool ShaderProgram::Key::operator==(const Key &other) const
{
	return this->vertexShader == other.vertexShader &&
		this->fragmentShader == other.fragmentShader;
}



ShaderProgram::ShaderProgram(const ShaderProgram::Key &shaderNames)
	: m_ShaderNames(shaderNames)
{
	m_TextureBindingMap[TextureType::DIFFUSE]   = std::vector<std::string>();
	m_TextureBindingMap[TextureType::SPECULAR]  = std::vector<std::string>();
	m_TextureBindingMap[TextureType::AMBIENT]   = std::vector<std::string>();
	m_TextureBindingMap[TextureType::SHININESS] = std::vector<std::string>();
}

void ShaderProgram::addTextureBinding(ShaderProgram::TextureType textureType, const std::string &name)
{
	m_TextureBindingMap[textureType].push_back(name);
}

bool ShaderProgram::operator==(const ShaderProgram &other) const
{
	return this->m_ShaderNames == other.m_ShaderNames;
}
bool ShaderProgram::operator==(const ShaderProgram::Key &other)const
{
	return this->m_ShaderNames == other;
}

//void ShaderProgram::writeVariable(const std::string &name, const glm::mat3 &data)
//{
//	GLSLVariable &variable = m_ActiveVariables.at(name);
//	if(variable.m_MemoryType == GLSLVariable::MemoryType::BLOCK_UNIFORM)
//	{
//
//	}
//	else if(variable.m_MemoryType == GLSLVariable::MemoryType::IN)
//	{
//
//	}
//	else if(variable.m_MemoryType == GLSLVariable::MemoryType::UNIFORM)
//	{
//
//	}
//
//}
//void ShaderProgram::writeVariable(const std::string &name, const glm::mat4 &data)
//{
//
//}
//void ShaderProgram::writeVariable(const std::string &name, const glm::vec3 &data)
//{
//
//}
//void ShaderProgram::writeVariable(const std::string &name, const glm::vec4 &data)
//{
//
//}
//
//
//void ShaderProgram::writeMVP(const glm::mat4 &MVP)
//{
//	GLSLVariable &variable = m_ActiveVariables.at("MVP");
//	if(variable.m_MemoryType == GLSLVariable::MemoryType::BLOCK_UNIFORM)
//	{
//
//	}
//}
//void ShaderProgram::writeNormalMatrix(const glm::mat3 &normalMatrix)
//{
//
//}
//void ShaderProgram::writeLight(const glm::vec3 &lightPosition)
//{
//
//}
//GLuint ShaderProgram::getPositionLocation()
//{
//
//}
//GLuint ShaderProgram::getNormalLocation()
//{
//
//}
//GLuint ShaderProgram::getTexCoordLocation()
//{
//
//}

GLuint ShaderProgram::getVariableLocation(const std::string &name) const
{
	std::map<std::string, GLuint>::const_iterator result = m_ActiveVariables.find(name);
	if(result != m_ActiveVariables.end())
		return result->second;
	//throw std::invalid_argument("the variable: " + name + " is not loaded in the shader: " + m_ShaderNames.vertexShader + "," + m_ShaderNames.fragmentShader);
	return -1;
}
