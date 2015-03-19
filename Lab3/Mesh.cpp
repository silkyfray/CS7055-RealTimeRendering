#include "Mesh.h"
#include "ShaderProgram.h"
#include <glm/gtc/type_ptr.hpp>

using std::vector;
using std::string;

Vertex::Vertex()
{
}

Vertex::Vertex(const glm::vec3 position)
	:m_Position(position)
{
}

void Vertex::operator=(const glm::vec3 &other)
{
	m_Position = other;
}

Mesh::Mesh(const std::vector<Vertex> &vertices,
	const std::vector<GLuint> &indices,
	const std::vector<Texture> &textures,
	ShaderProgram *shader)
	:m_Vertices(vertices), m_Indices(indices), m_Textures(textures)
{

}

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
	destroy();
}

void Mesh::destroy()
{
	glDeleteBuffers(1,&m_VBO);
	glDeleteBuffers(1,&m_EBO);
	glDeleteVertexArrays(1,&m_VAO);
}
//code adapted from http://learnopengl.com/#!Model-Loading/Mesh
void Mesh::createVAO(const ShaderProgram *shader)
{
	// Create buffers/arrays
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	if(m_Indices.size())
		glGenBuffers(1, &m_EBO);

	glBindVertexArray(m_VAO);
	// Load data into vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(Vertex), &m_Vertices[0], GL_STATIC_DRAW);

	if(m_Indices.size())
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(GLuint), &m_Indices[0], GL_STATIC_DRAW);
	}

	// Set the vertex attribute pointers
	// Vertex Positions
	GLuint position = shader->getVariableLocation("position");
	glEnableVertexAttribArray(position);
	glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
	// Vertex Normals
	GLuint normal = shader->getVariableLocation("normal");
	glEnableVertexAttribArray(normal);
	glVertexAttribPointer(normal, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, m_Normal));
	// Vertex Texture Coords
	GLuint texCoord = shader->getVariableLocation("texCoord");
	glEnableVertexAttribArray(texCoord);
	glVertexAttribPointer(texCoord, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, m_TexCoord));
	// Vertex Tangents
	GLuint tangent = shader->getVariableLocation("tangent");
	glEnableVertexAttribArray(tangent);
	glVertexAttribPointer(tangent, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, m_Tangent));
	// Vertex Bitangents
	GLuint bitangent = shader->getVariableLocation("bitangent");
	glEnableVertexAttribArray(bitangent);
	glVertexAttribPointer(bitangent, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, m_Bitangent));
	

	glBindVertexArray(0);

}



void Mesh::render(const ShaderProgram *shader, GLenum mode, bool drawElements) const
{
	
	//vector<string>::const_iterator diffuseTextures = shader->m_TextureBindingMap.at(ShaderProgram::TextureType::DIFFUSE).begin();
	//vector<string>::const_iterator specularTextures = shader->m_TextureBindingMap.at(ShaderProgram::TextureType::SPECULAR).begin();
	//vector<string>::const_iterator shininessTextures = shader->m_TextureBindingMap.at(ShaderProgram::TextureType::SHININESS).begin();
	//vector<string>::const_iterator ambientTextures = shader->m_TextureBindingMap.at(ShaderProgram::TextureType::AMBIENT).begin();
	//// Bind appropriate textures
	//for (GLuint i = 0; i < m_Textures.size(); i++)
	//{
	//	glActiveTexture(GL_TEXTURE0 + i); // Active proper texture unit before binding
	//	aiTextureType samplerType = m_Textures[i].m_Type;
	//	string textureName;
	//	if (samplerType == aiTextureType::aiTextureType_DIFFUSE)
	//	{
	//		textureName = *diffuseTextures++;
	//	}
	//	else if (samplerType == aiTextureType::aiTextureType_SPECULAR)
	//	{
	//		textureName = *specularTextures++;
	//	}
	//	else if (samplerType == aiTextureType::aiTextureType_SHININESS)
	//	{
	//		textureName = *shininessTextures++;
	//	}
	//	else if (samplerType == aiTextureType::aiTextureType_AMBIENT)
	//	{
	//		textureName = *ambientTextures++;
	//	}		// Now set the sampler to the correct texture unit number i
	//	glUniform1i(glGetUniformLocation(shader->m_Id, textureName.c_str()), i);
	//	// And finally bind the texture
	//	glBindTexture(GL_TEXTURE_2D, m_Textures[i].m_Id);
	//}
	//glActiveTexture(GL_TEXTURE0); //  set everything back to defaults once configured.

	// Draw mesh
	glBindVertexArray(m_VAO);
	if(drawElements)
		glDrawElements(mode, m_Indices.size(), GL_UNSIGNED_INT, 0);
	else
		glDrawArrays(mode, 0, m_Vertices.size());
	glBindVertexArray(0);

}

void Mesh::render(const ShaderProgram *shader) const
{
	render(shader,GL_TRIANGLES,true);
}

