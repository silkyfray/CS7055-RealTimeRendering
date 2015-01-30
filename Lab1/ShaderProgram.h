#pragma once
#include <glm/glm.hpp>
#include "precompiled.h"
#include <GL/glew.h>

//struct GLSLVariable
//{
//	enum class MemoryType{IN, OUT, UNIFORM, BLOCK_UNIFORM};
//	enum class StorageType{VEC3, VEC4, MAT3, MAT4};
//	
//	MemoryType m_MemoryType;
//	StorageType m_StorageType;
//	GLuint m_Location;
//
//};

/**@brief Represents a loaded shader program on the gpu
*/
struct ShaderProgram
{
public:
	
	struct Key{
		std::string vertexShader;
		std::string fragmentShader;
		Key(const std::string vertexShader, const std::string fragmentShader);
		bool operator==(const Key &other) const;
	};
	enum class TextureType{DIFFUSE, SPECULAR, AMBIENT, SHININESS};

	ShaderProgram(const ShaderProgram::Key &shaderNames);
	bool operator==(const ShaderProgram &other) const;
	bool operator==(const ShaderProgram::Key &other)const;
	void addTextureBinding(TextureType textureType, const std::string &name); 

	//void writeMVP(const glm::mat4 &MVP);
	//void writeNormalMatrix(const glm::mat3 &normalMatrix);
	//void writeLight(const glm::vec3 &lightPosition);
	//void writeVariable(const std::string &name, const glm::mat3 &data);
	//void writeVariable(const std::string &name, const glm::mat4 &data);
	//void writeVariable(const std::string &name, const glm::vec3 &data);
	//void writeVariable(const std::string &name, const glm::vec4 &data);
	//GLuint getPositionLocation();
	//GLuint getNormalLocation();
	//GLuint getTexCoordLocation();
	GLuint getVariableLocation(const std::string &name) const;
public:
	Key m_ShaderNames;
	GLuint m_Id;
	//map key : diffuse, specular, etc. and map value : list of names for that type
	typedef std::map<TextureType, std::vector<std::string> > TextureBindingMap;
	TextureBindingMap m_TextureBindingMap;

	std::map<std::string, GLuint> m_ActiveVariables;
};
