#pragma once
#include "Mesh.h"
#include "SQTTransform.h"
#include "Watercolor.h"

#include <assimp/Importer.hpp>
#include <glm/glm.hpp>

struct ShaderProgram;
struct aiScene;
struct aiNode;
class CubeMap;

/**
@brief A class that is used as the template to instantiate Object instances.
@details Each Object contain a pointer to its Model. Many Object instances can have the same underlying Model.
As such Model member variables are not allowed to be changed except the shader that it uses.
once after initialization. A Model owns the Assimp Scene that it loads. Disgards of the scene after construction once 
the relevant data structure have been loaded. Model also own the its Mesh objects so it manages the lifetime of Mesh.
Between the GameWorld -> Object -> Model -> Mesh -> Vertex hierarchy it is Model that associates with a given ShaderProgram.
some of the code is adapted from http://learnopengl.com/#!Model-Loading/Model
*/
class Model
{
public:
	/**@brief Contruct a model by giving the modelTable loaded from lua config file.
		 @details Usually it is the responsibility of ModelManager to initialize and load models
	 */
	Model(const std::string &name, const std::string &modelDir, ShaderProgram *shaderProgram);

	/**Deletes the Mesh objects of the model*/
	virtual ~Model();

	virtual void render(glm::mat4 modelMatrix, glm::mat4 viewMatrix, glm::mat4 projectionMatrix, glm::vec3 lightDirectionWorld) const;
	virtual void renderGouraud(ShaderProgram *shader, const glm::mat4 &modelMatrix, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix, glm::vec3 lightDirectionWorld) const;
	virtual void renderBlinnPhongTexture(ShaderProgram *shader, const glm::mat4 &modelMatrix, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix, glm::vec3 lightDirectionWorld) const;
	virtual void renderCookTor(ShaderProgram *shader, const glm::mat4 &modelMatrix, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix, glm::vec3 lightDirectionWorld) const;
	virtual void renderCel(ShaderProgram *celShader, ShaderProgram *celOutline, const glm::mat4 &modelMatrix, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix, glm::vec3 lightDirectionWorld, glm::vec4 lineColor, float lineTickness) const;
	virtual void renderCubeMap(ShaderProgram *shader, const CubeMap &cubeMap, const glm::mat4 &modelMatrix, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix) const;
	virtual void renderReflectRefract(ShaderProgram *shader, const glm::mat4 &modelMatrix, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix, glm::vec3 lightDirectionWorld,
		const CubeMap &cubeMap, float eta, float reflectionFactor) const;
	virtual void renderReflectRefract2(ShaderProgram *shader, const CubeMap &cubeMap, const glm::mat4 &modelMatrix, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix, glm::vec3 lightDirectionWorld,
		float eta[3], float fresnel) const;
	virtual void renderNormalMap(ShaderProgram *shader, const glm::mat4 &modelMatrix, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix, glm::vec3 lightDirectionWorld, glm::vec3 lightIntensity, bool renderDiffuse, bool renderNormalMap) const;
	virtual void renderReflectRefractNormalMap(ShaderProgram *shader, const CubeMap &cubeMap, const glm::mat4 &modelMatrix, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix, glm::vec3 lightDirectionWorld,
		float eta[3], float fresnel, /*normal map stuff*/glm::vec3 lightIntensity, float mixRRfactor) const;
	virtual void renderWatercolor(const WatercolorInfo &watercolorInfo, const glm::mat4 &modelMatrix, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix, glm::vec3 lightDirectionWorld) const;
	ShaderProgram* getShaderProgram() const;
	void changeShader(ShaderProgram *shaderProgram);
	void setMaterialRoughness(float value);
	void setMaterialRefraction(float value);
protected:
	/** 
	@brief Loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
	*/
	virtual void loadScene();

	/**Get a single Vertex at position @param i of @param mesh. */
	Vertex retrieveVertex(const aiMesh* mesh, int i);

	/**Get the indices in @param mesh and transfer to @param resultMesh*/
	void retrieveIndices(Mesh *resultMesh, const aiMesh *mesh);

	/**@brief Get the material properties stored in the loaded assimp model.
		@details currently supports ambient, diffuse,specular and shininess components
	*/
	void retrieveMaterials(Mesh *resultMesh, const aiMesh* mesh);

	/**@brief Multiplies all the vertices of the mesh by the (root transforms * mesh transform)
	*/
	void bakeTransform(Mesh *resultMesh, const aiNode* node);
	/**Companion function to retrieveMaterials*/
	aiColor4D loadMaterialProperties(const aiMaterial *mat, const char *aiType, unsigned int type, unsigned int idx);

	/** @brief Processes a node in a recursive fashion.
		@details Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
	*/
	virtual void processNode(const aiNode* node);

	/**Creates a newly allocated Mesh for each aiMesh. Fill the Mesh with relevant data (vertices, normals, etc.)*/
	virtual Mesh* processMesh(const aiMesh* mesh);
	
	/** @brief Checks all material textures of a given type and loads the textures if they're not loaded yet.
	 */
	std::vector<Texture> loadMaterialTextures(const aiMaterial* mat, const aiTextureType type);

	/**@brief Loads a single texture from a file.
		@details Relies on @link m_ModelDir to have been set beforehand which happens in the loadScene function*/
	GLint textureFromFile(const std::string &textureDir);


public:
	const std::string m_Name;
	std::vector<Mesh*> m_Meshes; //!< the array of meshes that are rendered
	ShaderProgram *m_ShaderProgram;

protected:
	//used by subclasses only
	Model(const std::string &name);

	Assimp::Importer m_Importer; 
	const aiScene *m_Scene; //!< freed at the end of the constructor
	std::string m_ModelDir;  //!< contains the folder directory of the model
	std::vector<Texture> m_TexturesLoaded; //!< structure to speed up loading textures which have been loaded before

};

