#include "precompiled.h"
#include "Model.h"
#include "ShaderManager.h"
#include "ShaderProgram.h"
#include "math_helper.h"
#include "CubeMap.h"

#include <SOIL.h>
#include <GL/glew.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using std::map;
using std::vector;
using std::string;
using std::endl;

Model::Model(const std::string &name, const std::string &modelDir, ShaderProgram *shaderProgram)
	:m_Name(name), m_ModelDir(modelDir), m_ShaderProgram(shaderProgram)
{
	loadScene();
	// Process ASSIMP's root node recursively
	processNode(m_Scene->mRootNode);
	m_Importer.FreeScene();
}


Model::~Model()
{
	std::vector<Mesh*>::iterator it = m_Meshes.begin();
	for (; it != m_Meshes.end(); ++it)
		delete *it;
}

void Model::render(glm::mat4 modelMatrix, glm::mat4 viewMatrix, glm::mat4 projectionMatrix, glm::vec3 lightDirectionWorld) const
{//GOURAUD, PHONG, CEL, TOON, COOKTOR, BLINNPHONG
	//switch (m_ShaderProgram->m_ShaderType)
	//{
	//case ShaderType::PHONG:
	//case ShaderType::GOURAUD:
	//case ShaderType::CEL:
	//case ShaderType::COOKTOR:
	//	renderCookTor(m_ShaderProgram, modelMatrix, viewMatrix, projectionMatrix, lightDirectionWorld);
	//case ShaderType::BLINNPHONG:
	//	renderGouraud(m_ShaderProgram, modelMatrix, viewMatrix, projectionMatrix, lightDirectionWorld);
	//}
}


void Model::renderGouraud(ShaderProgram *shader, const glm::mat4 &modelMatrix, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix, glm::vec3 lightDirectionWorld) const
{
	glUseProgram(shader->m_Id);
	glm::mat4 modelViewMatrix = viewMatrix * modelMatrix;
	glm::mat4 MVP = projectionMatrix * modelViewMatrix;
	glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(modelViewMatrix)));
	
	glUniformMatrix4fv(shader->getVariableLocation("MVP"), 1, GL_FALSE, glm::value_ptr(MVP));
	glUniformMatrix3fv(shader->getVariableLocation("normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));
	glUniformMatrix4fv(shader->getVariableLocation("viewModelMatrix"), 1, GL_FALSE, glm::value_ptr(modelViewMatrix));
	glm::vec3 lightDirectionEye = glm::mat3(viewMatrix) * lightDirectionWorld;
	glUniform3fv(shader->getVariableLocation("lightDirection"), 1, glm::value_ptr(lightDirectionEye));

	std::vector<Mesh*>::const_iterator it = m_Meshes.begin();
	for (; it != m_Meshes.end(); ++it)
	{
		glUniform4fv(shader->getVariableLocation("diffuse"), 1, glm::value_ptr((*it)->m_Material.diffuse));
		glUniform4fv(shader->getVariableLocation("ambient"), 1, glm::value_ptr((*it)->m_Material.ambient));
		glUniform4fv(shader->getVariableLocation("specular"), 1, glm::value_ptr((*it)->m_Material.specular));
		glUniform1fv(shader->getVariableLocation("shininess"), 1, &((*it)->m_Material.shininess));
		(*it)->render(shader);
	}

}

void Model::renderBlinnPhongTexture(ShaderProgram *shader, const glm::mat4 &modelMatrix, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix, glm::vec3 lightDirectionWorld) const
{
	glUseProgram(shader->m_Id);
	glm::mat4 modelViewMatrix = viewMatrix * modelMatrix;
	glm::mat4 MVP = projectionMatrix * modelViewMatrix;
	glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(modelViewMatrix)));
	
	glUniformMatrix4fv(shader->getVariableLocation("MVP"), 1, GL_FALSE, glm::value_ptr(MVP));
	glUniformMatrix3fv(shader->getVariableLocation("normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));
	glUniformMatrix4fv(shader->getVariableLocation("viewModelMatrix"), 1, GL_FALSE, glm::value_ptr(modelViewMatrix));
	glm::vec3 lightDirectionEye = glm::mat3(viewMatrix) * lightDirectionWorld;
	glUniform3fv(shader->getVariableLocation("lightDirection"), 1, glm::value_ptr(lightDirectionEye));

	std::vector<Mesh*>::const_iterator it = m_Meshes.begin();
	for (; it != m_Meshes.end(); ++it)
	{
		glActiveTexture(GL_TEXTURE0); // Active proper texture unit before binding
		// Now set the sampler to the correct texture unit number i
		glUniform1i(shader->getVariableLocation("texture_diffuse1"), 0);
		// And finally bind the texture
		glBindTexture(GL_TEXTURE_2D, (*it)->m_Textures[0].m_Id);

		glUniform4fv(shader->getVariableLocation("ambient"), 1, glm::value_ptr((*it)->m_Material.ambient));
		glUniform4fv(shader->getVariableLocation("specular"), 1, glm::value_ptr((*it)->m_Material.specular));
		glUniform1fv(shader->getVariableLocation("shininess"), 1, &((*it)->m_Material.shininess));
		(*it)->render(shader);
	}
}

void Model::renderCookTor(ShaderProgram *shader, const glm::mat4 &modelMatrix, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix, glm::vec3 lightDirectionWorld) const
{
	glUseProgram(shader->m_Id);
	glm::mat4 modelViewMatrix = viewMatrix * modelMatrix;
	glm::mat4 MVP = projectionMatrix * modelViewMatrix;
	glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(modelViewMatrix)));
	
	glUniformMatrix4fv(shader->getVariableLocation("MVP"), 1, GL_FALSE, glm::value_ptr(MVP));
	glUniformMatrix3fv(shader->getVariableLocation("normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));
	glUniformMatrix4fv(shader->getVariableLocation("viewModelMatrix"), 1, GL_FALSE, glm::value_ptr(modelViewMatrix));
	glm::vec3 lightDirectionEye = glm::mat3(viewMatrix) * lightDirectionWorld;
	glUniform3fv(shader->getVariableLocation("lightDirection"), 1, glm::value_ptr(lightDirectionEye));

	std::vector<Mesh*>::const_iterator it = m_Meshes.begin();
	for (; it != m_Meshes.end(); ++it)
	{
		glUniform4fv(shader->getVariableLocation("diffuse"), 1, glm::value_ptr((*it)->m_Material.diffuse));
		glUniform4fv(shader->getVariableLocation("ambient"), 1, glm::value_ptr((*it)->m_Material.ambient));
		glUniform4fv(shader->getVariableLocation("specular"), 1, glm::value_ptr((*it)->m_Material.specular));
		glUniform1fv(shader->getVariableLocation("roughness"), 1, &((*it)->m_Material.roughness));
		glUniform1fv(shader->getVariableLocation("refraction"), 1, &((*it)->m_Material.refraction));
		(*it)->render(shader);
	}
}

void Model::renderCel(ShaderProgram *celShader, ShaderProgram *celOutline, const glm::mat4 &modelMatrix, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix, glm::vec3 lightDirectionWorld, glm::vec4 lineColor, float lineTickness) const
{

	glm::mat4 modelViewMatrix = viewMatrix * modelMatrix;
	glm::mat4 MVP = projectionMatrix * modelViewMatrix;
	glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(modelViewMatrix)));
	std::vector<Mesh*>::const_iterator it = m_Meshes.begin();

	//glFrontFace(GL_CW);
	////render the outline first
	//glUseProgram(celOutline->m_Id);
	//glUniformMatrix4fv(celOutline->getVariableLocation("MVP"), 1, GL_FALSE, glm::value_ptr(MVP));
	//glUniformMatrix4fv(celOutline->getVariableLocation("projectionMatrix"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	//glUniformMatrix3fv(celOutline->getVariableLocation("normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));
	//glUniform1fv(celOutline->getVariableLocation("lineTickness"), 1, (GLfloat*)&lineTickness);
	//glUniform4fv(celOutline->getVariableLocation("lineColor"), 1, glm::value_ptr(lineColor));
	//for (; it != m_Meshes.end(); ++it)
	//{
	//	(*it)->render(celOutline);
	//}
	//glFrontFace(GL_CCW);
	glUseProgram(celShader->m_Id);
	//render the cel shading second
	glUniformMatrix4fv(celShader->getVariableLocation("MVP"), 1, GL_FALSE, glm::value_ptr(MVP));
	glUniformMatrix3fv(celShader->getVariableLocation("normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));
	glUniformMatrix4fv(celShader->getVariableLocation("viewModelMatrix"), 1, GL_FALSE, glm::value_ptr(modelViewMatrix));
	glm::vec3 lightDirectionEye = glm::mat3(viewMatrix) * lightDirectionWorld;
	glUniform3fv(celShader->getVariableLocation("lightDirection"), 1, glm::value_ptr(lightDirectionEye));
	std::vector<Mesh*>::const_iterator it2 = m_Meshes.begin();
	for (; it2 != m_Meshes.end(); ++it2)
	{
		glUniform4fv(celShader->getVariableLocation("diffuse"), 1, glm::value_ptr((*it2)->m_Material.diffuse));
		glUniform4fv(celShader->getVariableLocation("ambient"), 1, glm::value_ptr((*it2)->m_Material.ambient));
		(*it2)->render(celShader);
	}

}

void Model::renderCubeMap(ShaderProgram *shader, const CubeMap &cubeMap, const glm::mat4 &modelMatrix, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix) const
{
	glDisable(GL_CULL_FACE);
	glm::mat4 modelViewMatrix = viewMatrix * modelMatrix;
	glm::mat4 MVPcube = projectionMatrix * viewMatrix * glm::scale(glm::mat4(), glm::vec3(cubeMap.m_Scale));
	glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(modelViewMatrix)));

	glUseProgram(shader->m_Id);
	cubeMap.writeCubeMapTexture(shader, "Cubemap", 0);
	glUniformMatrix4fv(shader->getVariableLocation("MVP"), 1, GL_FALSE, glm::value_ptr(MVPcube));
	cubeMap.m_Model->m_Meshes[0]->render(shader);
}

void Model::renderNormalMap(ShaderProgram *shader, const glm::mat4 &modelMatrix, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix, glm::vec3 lightDirectionWorld, glm::vec3 lightIntensity, bool renderDiffuse, bool renderNormalMap) const
{
	glUseProgram(shader->m_Id);
	glm::mat4 modelViewMatrix = viewMatrix * modelMatrix;
	glm::mat4 MVP = projectionMatrix * modelViewMatrix;
	glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(modelViewMatrix)));

	glUniformMatrix4fv(shader->getVariableLocation("MVP"), 1, GL_FALSE, glm::value_ptr(MVP));
	glUniformMatrix4fv(shader->getVariableLocation("ModelViewMatrix"), 1, GL_FALSE, glm::value_ptr(modelViewMatrix));
	glUniformMatrix3fv(shader->getVariableLocation("NormalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));
	glUniformMatrix4fv(shader->getVariableLocation("ProjectionMatrix"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	glm::vec3 lightDirectionEye = glm::mat3(viewMatrix) * lightDirectionWorld;
	glUniform3fv(shader->getVariableLocation("Light.Direction"), 1, glm::value_ptr(lightDirectionEye));
	glUniform3fv(shader->getVariableLocation("Light.Intensity"), 1, glm::value_ptr(lightIntensity));

	std::vector<Mesh*>::const_iterator it = m_Meshes.begin();
	for (; it != m_Meshes.end(); ++it)
	{
		std::vector<Texture>::const_iterator texture = (*it)->m_Textures.begin();
		for (; texture != (*it)->m_Textures.end(); ++texture)
		{

			if(texture->m_Type == aiTextureType_DIFFUSE)
			{
				if(renderDiffuse)
				{
					glUniform1i(shader->getVariableLocation("renderDiffuse"), 1);
					glActiveTexture(GL_TEXTURE0); // Active proper texture unit before binding
					// Now set the sampler to the correct texture unit number i
					glUniform1i(shader->getVariableLocation("ColorTex"), 0);
					// And finally bind the texture
					glBindTexture(GL_TEXTURE_2D, texture->m_Id);
				}
				else
				{
					glUniform1i(shader->getVariableLocation("renderDiffuse"), 0);
				}
			}
			else if(texture->m_Type == aiTextureType_NORMALS)
			{
				if(renderNormalMap)
				{
					glUniform1i(shader->getVariableLocation("renderNormalMap"), 1);
					glActiveTexture(GL_TEXTURE0 + 1); // Active proper texture unit before binding
					// Now set the sampler to the correct texture unit number i
					glUniform1i(shader->getVariableLocation("NormalMapTex"), 1);
					// And finally bind the texture
					glBindTexture(GL_TEXTURE_2D, texture->m_Id);
				}
				else
				{
					glUniform1i(shader->getVariableLocation("renderNormalMap"), 0);
				}
			}
			else if(texture->m_Type == aiTextureType_LIGHTMAP)
			{
				glActiveTexture(GL_TEXTURE0 + 2); // Active proper texture unit before binding
				// Now set the sampler to the correct texture unit number i
				glUniform1i(shader->getVariableLocation("LightMapTex"), 2);
				// And finally bind the texture
				glBindTexture(GL_TEXTURE_2D, texture->m_Id);
			}
		}

		glUniform4fv(shader->getVariableLocation("Material.Ka"), 1, glm::value_ptr((*it)->m_Material.ambient));
		glUniform4fv(shader->getVariableLocation("Material.Ks"), 1, glm::value_ptr((*it)->m_Material.specular));
		glUniform1fv(shader->getVariableLocation("Material.Shininess"), 1, &((*it)->m_Material.shininess));
		(*it)->render(shader);
	}

	glActiveTexture(GL_TEXTURE0);
	
}


void Model::renderReflectRefract(ShaderProgram *shader, const glm::mat4 &modelMatrix, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix, glm::vec3 lightDirectionWorld,
								 const CubeMap &cubeMap, float eta, float reflectionFactor) const
{
	glUseProgram(shader->m_Id);
	glm::mat4 modelViewMatrix = viewMatrix * modelMatrix;
	glm::mat4 MVPmodel = projectionMatrix * modelViewMatrix;
	glm::mat4 MVPcube = projectionMatrix * viewMatrix * glm::scale(glm::mat4(), glm::vec3(cubeMap.m_Scale));
	glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(modelViewMatrix)));
	glm::vec3 worldCameraPosition = glm::vec3(viewMatrix[3]);

	glUniformMatrix3fv(shader->getVariableLocation("NormalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));
	glUniformMatrix4fv(shader->getVariableLocation("ModelViewMatrix"), 1, GL_FALSE, glm::value_ptr(modelViewMatrix));
	glUniformMatrix4fv(shader->getVariableLocation("ProjectionMatrix"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniform3fv(shader->getVariableLocation("WorldCameraPosition"), 1, glm::value_ptr(worldCameraPosition));
	glUniform1f(shader->getVariableLocation("Material.Eta"), eta);
	glUniform1f(shader->getVariableLocation("Material.ReflectionFactor"), reflectionFactor);
	//glUniformMatrix4fv(shader->getVariableLocation("ModelMatrix"), 1, GL_FALSE, glm::value_ptr(projectionMatrix * viewMatrix * modelMatrix));

	cubeMap.writeCubeMapTexture(shader, "CubeMapTex", 0);
	
	glUniformMatrix4fv(shader->getVariableLocation("MVP"), 1, GL_FALSE, glm::value_ptr(MVPcube));
	glUniform1ui(shader->getVariableLocation("DrawSkyBox"), 1);
	cubeMap.m_Model->m_Meshes[0]->render(shader);
	
	glUniformMatrix4fv(shader->getVariableLocation("MVP"), 1, GL_FALSE, glm::value_ptr(MVPmodel));
	glUniformMatrix4fv(shader->getVariableLocation("ModelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glUniform1ui(shader->getVariableLocation("DrawSkyBox"), 0);
	std::vector<Mesh*>::const_iterator it = m_Meshes.begin();
	for (; it != m_Meshes.end(); ++it)
	{
		(*it)->render(shader);
	}
	//cubeMap.m_Model->render(shader);

}

void Model::renderReflectRefract2(ShaderProgram *shader, const CubeMap &cubeMap, const glm::mat4 &modelMatrix, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix, glm::vec3 lightDirectionWorld,
								  float eta[3], float fresnel) const
{
	glm::mat4 modelViewMatrix = viewMatrix * modelMatrix;
	glm::mat4 MVPmodel = projectionMatrix * modelViewMatrix;
	glm::mat4 MVPcube = projectionMatrix * viewMatrix * glm::scale(glm::mat4(), glm::vec3(cubeMap.m_Scale));
	glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(modelViewMatrix)));
	glEnable(GL_CULL_FACE);
	glUseProgram(shader->m_Id);
	glUniformMatrix3fv(shader->getVariableLocation("NormalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));
	glUniformMatrix4fv(shader->getVariableLocation("ModelViewMatrix"), 1, GL_FALSE, glm::value_ptr(modelViewMatrix));
	glUniformMatrix4fv(shader->getVariableLocation("MVP"), 1, GL_FALSE, glm::value_ptr(MVPmodel));
	glUniform3f(shader->getVariableLocation("Eta"), eta[0], eta[1], eta[2]);
	glUniform1f(shader->getVariableLocation("FresnelPower"), fresnel);

	float averageEta = (eta[0] + eta[1] + eta[2])/ 3.0f;
	float F = ((1.0-averageEta) * (1.0-averageEta)) / ((1.0+averageEta) * (1.0+averageEta));
	glUniform1f(shader->getVariableLocation("F"), F);
	//glUniformMatrix4fv(shader->getVariableLocation("ModelMatrix"), 1, GL_FALSE, glm::value_ptr(projectionMatrix * viewMatrix * modelMatrix));
	cubeMap.writeCubeMapTexture(shader, "Cubemap", 0);
	
	glUniformMatrix4fv(shader->getVariableLocation("ModelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
	std::vector<Mesh*>::const_iterator it = m_Meshes.begin();
	for (; it != m_Meshes.end(); ++it)
	{
		(*it)->render(shader);
	}

}

void Model::renderReflectRefractNormalMap(ShaderProgram *shader, const CubeMap &cubeMap, const glm::mat4 &modelMatrix, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix, glm::vec3 lightDirectionWorld,
		float eta[3], float fresnel, /*normal map stuff*/glm::vec3 lightIntensity, float mixRRfactor) const
{

	glm::mat4 modelViewMatrix = viewMatrix * modelMatrix;
	glm::mat4 MVPmodel = projectionMatrix * modelViewMatrix;
	glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(modelViewMatrix)));
	glEnable(GL_CULL_FACE);
	glUseProgram(shader->m_Id);
	glUniformMatrix3fv(shader->getVariableLocation("NormalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));
	glUniformMatrix4fv(shader->getVariableLocation("ModelViewMatrix"), 1, GL_FALSE, glm::value_ptr(modelViewMatrix));
	glUniformMatrix4fv(shader->getVariableLocation("MVP"), 1, GL_FALSE, glm::value_ptr(MVPmodel));
	glUniform3f(shader->getVariableLocation("Eta"), eta[0], eta[1], eta[2]);
	glUniform1f(shader->getVariableLocation("FresnelPower"), fresnel);
	glUniform1fv(shader->getVariableLocation("mixRRfactor"), 1, &mixRRfactor);

	float averageEta = (eta[0] + eta[1] + eta[2])/ 3.0f;
	float F = ((1.0-averageEta) * (1.0-averageEta)) / ((1.0+averageEta) * (1.0+averageEta));
	glUniform1f(shader->getVariableLocation("F"), F);
	//glUniformMatrix4fv(shader->getVariableLocation("ModelMatrix"), 1, GL_FALSE, glm::value_ptr(projectionMatrix * viewMatrix * modelMatrix));
	cubeMap.writeCubeMapTexture(shader, "Cubemap", 3);
	
	glUniformMatrix4fv(shader->getVariableLocation("ModelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));

	//normal map stuff

	glUniformMatrix4fv(shader->getVariableLocation("ProjectionMatrix"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	glm::vec3 lightDirectionEye = glm::mat3(viewMatrix) * lightDirectionWorld;
	glUniform3fv(shader->getVariableLocation("Light.Direction"), 1, glm::value_ptr(lightDirectionEye));
	glUniform3fv(shader->getVariableLocation("Light.Intensity"), 1, glm::value_ptr(lightIntensity));

	std::vector<Mesh*>::const_iterator it = m_Meshes.begin();
	for (; it != m_Meshes.end(); ++it)
	{
		std::vector<Texture>::const_iterator texture = (*it)->m_Textures.begin();
		for (; texture != (*it)->m_Textures.end(); ++texture)
		{

			if(texture->m_Type == aiTextureType_DIFFUSE)
			{
				glActiveTexture(GL_TEXTURE0); // Active proper texture unit before binding
				// Now set the sampler to the correct texture unit number i
				glUniform1i(shader->getVariableLocation("ColorTex"), 0);
				// And finally bind the texture
				glBindTexture(GL_TEXTURE_2D, texture->m_Id);
			}
			else if(texture->m_Type == aiTextureType_NORMALS)
			{
				glActiveTexture(GL_TEXTURE0 + 1); // Active proper texture unit before binding
				// Now set the sampler to the correct texture unit number i
				glUniform1i(shader->getVariableLocation("NormalMapTex"), 1);
				// And finally bind the texture
				glBindTexture(GL_TEXTURE_2D, texture->m_Id);
			}
			else if(texture->m_Type == aiTextureType_LIGHTMAP)
			{
				glActiveTexture(GL_TEXTURE0 + 2); // Active proper texture unit before binding
				// Now set the sampler to the correct texture unit number i
				glUniform1i(shader->getVariableLocation("LightMapTex"), 2);
				// And finally bind the texture
				glBindTexture(GL_TEXTURE_2D, texture->m_Id);
			}
		}

		glUniform4fv(shader->getVariableLocation("Material.Ka"), 1, glm::value_ptr((*it)->m_Material.ambient));
		glUniform4fv(shader->getVariableLocation("Material.Ks"), 1, glm::value_ptr((*it)->m_Material.specular));
		glUniform1fv(shader->getVariableLocation("Material.Shininess"), 1, &((*it)->m_Material.shininess));

		(*it)->render(shader);
	}
	glActiveTexture(GL_TEXTURE0);

}


void Model::changeShader(ShaderProgram *shaderProgram)
{
	m_ShaderProgram = shaderProgram;
	//std::vector<Mesh*>::const_iterator it = m_Meshes.begin();
	//for (; it != m_Meshes.end(); ++it)
	//	(*it)->createVAO(m_ShaderProgram);
}

void Model::loadScene()
{
	// Read file via ASSIMP
	m_Scene = m_Importer.ReadFile(m_ModelDir, aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	// Check for errors
	if (!m_Scene || m_Scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !m_Scene->mRootNode)
	{
		std::cout << "Assimp " << m_Importer.GetErrorString() << endl;
	}	
	//save off the directory where the model is saved because later will need it for loading textures relative to that directory
	m_ModelDir = m_ModelDir.substr(0, m_ModelDir.find_last_of('/'));

}

ShaderProgram* Model::getShaderProgram() const
{
	return m_ShaderProgram;
}
void Model::processNode(const aiNode* node)
{
	for (GLuint i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh *mesh = m_Scene->mMeshes[node->mMeshes[i]];
		Mesh *resultMesh = processMesh(mesh);
		resultMesh->createVAO(m_ShaderProgram);

		//allrighty. what is this, you ask. Even static meshes can have a transformation in their corresponding assimp Node. It was an oversight of me to ignore this transformation when designing the class so the following is a patch up
		//bakeTransform(resultMesh,node);
		m_Meshes.push_back(resultMesh);

	}
	for (GLuint i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i]);
	}
}

Mesh* Model::processMesh(const aiMesh* mesh)
{
	Mesh* resultMesh = new Mesh();
	for (GLuint i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex = retrieveVertex(mesh, i);
		resultMesh->m_Vertices.push_back(vertex);
	}

	retrieveIndices(resultMesh, mesh);
	retrieveMaterials(resultMesh, mesh);

	return resultMesh;
}

Vertex Model::retrieveVertex(const aiMesh* mesh, int i)
{
	Vertex vertex;
	glm::vec3 v;
	//Positions
	vertex.m_Position.x = mesh->mVertices[i].x;
	vertex.m_Position.y = mesh->mVertices[i].y;
	vertex.m_Position.z = mesh->mVertices[i].z;

	if (mesh->HasNormals())
	{
		// Normals
		vertex.m_Normal.x = mesh->mNormals[i].x;
		vertex.m_Normal.y = mesh->mNormals[i].y;
		vertex.m_Normal.z = mesh->mNormals[i].z;
	}
	if (mesh->mTextureCoords[0]) // check if the mesh contains texture coordinates
	{
		// A vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
		// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
		vertex.m_TexCoord.x = mesh->mTextureCoords[0][i].x;
		vertex.m_TexCoord.y = mesh->mTextureCoords[0][i].y;
	}
	else
	{
		vertex.m_TexCoord = glm::vec2(0.0f, 0.0f);
	}

	if (mesh->HasTangentsAndBitangents())
	{
		vertex.m_Tangent.x = mesh->mTangents[i].x;
		vertex.m_Tangent.y = mesh->mTangents[i].y;
		vertex.m_Tangent.z = mesh->mTangents[i].z;

		vertex.m_Bitangent.x = mesh->mBitangents[i].x;
		vertex.m_Bitangent.y = mesh->mBitangents[i].y;
		vertex.m_Bitangent.z = mesh->mBitangents[i].z;
	}
  	return vertex;

}

void Model::retrieveIndices(Mesh *resultMesh, const aiMesh* mesh)
{
	//retrieve the mesh faces a.k.a primitives
	for (GLuint i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		// Retrieve all indices of the face and store them in the indices vector
		for (GLuint j = 0; j < face.mNumIndices; j++)
			resultMesh->m_Indices.push_back(face.mIndices[j]);

	}

}

void Model::retrieveMaterials(Mesh *resultMesh, const aiMesh* mesh)
{

	//Process materials
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial *material = m_Scene->mMaterials[mesh->mMaterialIndex];
		const aiTextureType textureTypes[] =
		{
			aiTextureType_AMBIENT,
			aiTextureType_DIFFUSE,
			aiTextureType_DISPLACEMENT,
			aiTextureType_EMISSIVE,
			aiTextureType_HEIGHT,
			aiTextureType_LIGHTMAP,
			aiTextureType_NONE,
			aiTextureType_NORMALS,
			aiTextureType_OPACITY,
			aiTextureType_REFLECTION,
			aiTextureType_SHININESS,
			aiTextureType_SPECULAR,
			aiTextureType_UNKNOWN
		};
		for (int i = 0; i < sizeof(textureTypes) / sizeof(textureTypes[0]); i++)
		{
			vector<Texture> maps = loadMaterialTextures(material, textureTypes[i]);
			resultMesh->m_Textures.insert(resultMesh->m_Textures.end(), maps.begin(), maps.end());
		}
		aiColor4D ambient, diffuse, specular;
		float shininess;
		aiGetMaterialColor(material,AI_MATKEY_COLOR_AMBIENT, &ambient);
		aiGetMaterialColor(material,AI_MATKEY_COLOR_DIFFUSE, &diffuse);
		aiGetMaterialColor(material,AI_MATKEY_COLOR_SPECULAR, &specular);
		aiGetMaterialFloat(material,AI_MATKEY_SHININESS, &shininess);
		resultMesh->m_Material.ambient = convertToGLMVec4(ambient);
		resultMesh->m_Material.diffuse = convertToGLMVec4(diffuse);
		resultMesh->m_Material.specular = convertToGLMVec4(specular);
		resultMesh->m_Material.shininess = shininess; 
	}
}

void Model::bakeTransform(Mesh *resultMesh, const aiNode *node)
{
	glm::mat4 rootTransform = convertToGLMMat4(m_Scene->mRootNode->mTransformation);
	for (int i = 0; i < resultMesh->m_Vertices.size(); i++)
	{
		glm::mat4 transform = rootTransform * convertToGLMMat4(node->mTransformation);
		//position
		glm::vec4 result = transform * glm::vec4(resultMesh->m_Vertices[i].m_Position,1.0f);
		resultMesh->m_Vertices[i].m_Position = glm::vec3(result.x, result.y, result.z); 
		//normal
		result = glm::transpose(glm::inverse(transform)) * glm::vec4(resultMesh->m_Vertices[i].m_Normal,1.0f);
		result = glm::normalize(result);
		resultMesh->m_Vertices[i].m_Normal = glm::vec3(result.x, result.y, result.z);
	}
}
aiColor4D Model::loadMaterialProperties(const aiMaterial *mat, const char *aiType, unsigned int type, unsigned int idx)
{
	aiColor4D value(0, 0, 0, 0);
	if (AI_SUCCESS == mat->Get(aiType, type, idx, value) && value != aiColor4D(0, 0, 0, 0))
	{
		//std::cout << "Found material property " << aiType << " : (" << value.r << "," << value.g << "," << value.b << "," << value.a << ")";
	}
	return value;
}

vector<Texture> Model::loadMaterialTextures(const aiMaterial* mat, const aiTextureType type)
{
	vector<Texture> textures;
	unsigned int textureCount = mat->GetTextureCount(type);
	for (GLuint i = 0; i < textureCount; i++)
	{
		aiString str;
		//retrieve the texture relative directory
		mat->GetTexture(type, i, &str);
		// Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		GLboolean skip = false;
		for (GLuint j = 0; j < m_TexturesLoaded.size(); j++)
		{
			if (m_TexturesLoaded[j].m_Path == str)
			{
				textures.push_back(m_TexturesLoaded[j]);
				skip = true; // A texture with the same filepath has already been loaded
				break;
			}
		}
		if (!skip)
		{   // If texture hasn't been loaded already, load it
			Texture texture;
			texture.m_Id = textureFromFile(str.C_Str());
			texture.m_Type = type;
			texture.m_Path = str;
			textures.push_back(texture);
			m_TexturesLoaded.push_back(texture);  // Store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
		}
	}
	return textures;
}

GLint Model::textureFromFile(const string &textureDir)
{
	string filepath = m_ModelDir + "/" + textureDir;
	GLuint textureID;
	glGenTextures(1, &textureID);
	int width, height;
	unsigned char* image = SOIL_load_image(filepath.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
	if (!image)
	{
		std::cout << "Could not load image : " << textureDir;
	}
	// Assign texture to ID
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	SOIL_free_image_data(image);
	return textureID;
}

void Model::setMaterialRoughness(float value)
{
	for(auto &mesh : m_Meshes)
	{
		mesh->m_Material.roughness = value;
	}
}
void Model::setMaterialRefraction(float value)
{
	for(auto &mesh : m_Meshes)
	{
		mesh->m_Material.refraction = value;
	}
}



