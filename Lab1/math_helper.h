#pragma once
#include <assimp/matrix4x4.h>
#include <assimp/color4.h>
#include <assimp/quaternion.h>
#include <assimp/vector3.h>

#include <glm/glm.hpp>

const float PI = 3.1415927f;
#include "SQTTransform.h"

#include <cstdlib>
#include <ctime>

//glm : column major , assimp: row major
/**@brief Convert an assimp matrix to glm matrix
*/
glm::mat4 convertToGLMMat4(const aiMatrix4x4 &from);
/**@brief Convert a glm to assimp matrix*/
inline aiMatrix4x4 convertToAiMatrix4x4(const glm::mat4  &from)
{
	aiMatrix4x4 to;
	to.a1 = from[0][0]; to.a2 = from[1][0];	to.a3 = from[2][0]; to.a4 = from[3][0];
	to.b1 = from[0][1]; to.b2 = from[1][1];	to.b3 = from[2][1]; to.b4 = from[3][1];
	to.c1 = from[0][2]; to.c2 = from[1][2];	to.c3 = from[2][2]; to.c4 = from[3][2];
	to.d1 = from[0][3]; to.d2 = from[1][3];	to.d3 = from[2][3]; to.d4 = from[3][3];
	return to;
}

/**@brief Convert an assimp color container to glm 4d float vector*/
glm::vec4 convertToGLMVec4(const aiColor4D &from);

/**@brief Convert an assimp 3d vector to glm 3d float vector*/
glm::vec3 convertToGLMVec3(const aiVector3D &from);

/**@brief	Convert an assimp quaternion to glm vector to glm quaternion*/
glm::quat convertToGLMQuat(const aiQuaternion &from);


/**@brief convert from matrix to sqt transform representation CAUTION: preserves transformations only if uniform scale is present*/
inline SQTTransform convertToSQTTransform(const aiMatrix4x4 &from)
{
	SQTTransform to;
	aiVector3D scale;
	aiQuaternion rotation;
	aiVector3D position;
	from.Decompose(scale, rotation, position);
	
	glm::vec3 glmScale = convertToGLMVec3(scale);
	glm::quat glmRotation = convertToGLMQuat(rotation);
	glm::vec3 glmTranslation = convertToGLMVec3(position);
	to = SQTTransform(glmTranslation, glmScale, glmRotation);
	return to;
}
/**@brief convert from matrix to sqt transform representation CAUTION: preserves transformations only if uniform scale is present*/
SQTTransform convertToSQTTransform(const glm::mat4 &from);

float radiansToDegrees(float radians);

float degreeToRadians(float degrees);

glm::quat eulerToQuaternion(glm::vec3 eulerAngles);

glm::mat4 scalarMultiplication(float scalar, glm::mat4 matrix);

glm::vec3 dampen(glm::quat from, glm::vec3 dampVec);

float randomNumber(float Min, float Max);