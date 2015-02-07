#pragma once

#include "precompiled.h"

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <assimp/scene.h>

/** @brief The vertex structure to be written to the gpu. Note the resulting packed layout*/
struct Vertex
{
	Vertex();
	Vertex(const glm::vec3 position);
	void operator=(const glm::vec3 &other);
	glm::vec3 m_Position; //!< position in model space
	glm::vec3 m_Normal;//!< normal of vertex in model space
	glm::vec2 m_TexCoord; //!< uv coordinates for vertex
};

/**@brief The texture structure to be written to the gpu. Texture stored at Mesh level*/
struct Texture
{
	GLuint m_Id; //!< the texture id in the buffer
	aiTextureType m_Type; //!< diffuse, specular, normal map, etc.
	aiString m_Path; //!< the file path of the image
};

/**@brief Refer to one of the fragment shaders to see the layout of the material structure. 
The fragment shader for the mesh may use texture and/or material properties */
struct Material
{
	//various material properties needed for lighting
	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;
	float shininess;
	float roughness;
	float refraction;
};

struct ShaderProgram;

/**@brief Encapsulates the buffers and texture handling for a single mesh.
@todo proper cleanup of buffer object in destructor
*/
struct Mesh
{
public:
	/** Construct a Mesh from vertices, indices and textures
		@param shader is the associated ShaderProgram for that Mesh
	*/
	Mesh(const std::vector<Vertex> &vertices,
		const std::vector<GLuint> &indices,
		const std::vector<Texture> &textures,
		ShaderProgram *shader);

	Mesh();
	virtual ~Mesh();
	/**@brief Deletes buffers*/
	void destroy();

	/**@brief Create a VAO for @param shader from the already loaded vertices */
	virtual void createVAO(const ShaderProgram *shader);
	/**@brief Writes material properties, textures and draws the triangles. Defaults to GL_TRIANGLES*/
	virtual void render(const ShaderProgram *shader) const;	
	/**@brief Writes material properties, textures and draws the triangles*/
	virtual void render(const ShaderProgram *shader,GLenum mode, bool drawElements) const;


public:
	std::vector<Vertex> m_Vertices;
	std::vector<GLuint> m_Indices;
	std::vector<Texture> m_Textures;
	Material m_Material;

protected:
	GLuint m_VAO;
	GLuint m_VBO;//!< single VAO initialised with subbuffer data
	GLuint m_EBO; //!< index buffer
};


