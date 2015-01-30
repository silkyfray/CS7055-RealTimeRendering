#pragma once
#include "math_helper.h"

//glm : column major , assimp: row major
/**@brief Convert an assimp matrix to glm matrix
*/
glm::mat4 convertToGLMMat4(const aiMatrix4x4 &from)
{
	glm::mat4 to;
	//the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
	to[0][0] = from.a1; to[1][0] = from.a2;	to[2][0] = from.a3; to[3][0] = from.a4;
	to[0][1] = from.b1; to[1][1] = from.b2;	to[2][1] = from.b3; to[3][1] = from.b4;
	to[0][2] = from.c1; to[1][2] = from.c2;	to[2][2] = from.c3; to[3][2] = from.c4;
	to[0][3] = from.d1; to[1][3] = from.d2;	to[2][3] = from.d3; to[3][3] = from.d4;
	return to;
}


/**@brief Convert an assimp color container to glm 4d float vector*/
glm::vec4 convertToGLMVec4(const aiColor4D &from)
{
	glm::vec4 to;
	to.r = from.r;
	to.g = from.g;
	to.b = from.b;
	to.a = from.a;
	return to;
}

/**@brief Convert an assimp 3d vector to glm 3d float vector*/
glm::vec3 convertToGLMVec3(const aiVector3D &from)
{
	glm::vec3 to;
	to.x = from.x;
	to.y = from.y;
	to.z = from.z;
	return to;
}

/**@brief	Convert an assimp quaternion to glm vector to glm quaternion*/
glm::quat convertToGLMQuat(const aiQuaternion &from)
{
	glm::quat to;
	to.x = from.x;
	to.y = from.y;
	to.z = from.z;
	to.w = from.w;
	return to;
}


/**@brief convert from matrix to sqt transform representation CAUTION: preserves transformations only if uniform scale is present*/
SQTTransform convertToSQTTransform(const glm::mat4 &from)
{
	SQTTransform to;
	//extract translation
	glm::vec3 position;
	position.x = from[3].x;
	position.y = from[3].y;
	position.z = from[3].z;

	//extract scale
	glm::vec3 scale;
	scale.x = glm::length(from[0]);
	scale.y = glm::length(from[1]);
	scale.z = glm::length(from[2]);

	if (glm::determinant(from) < 0)
	{
		scale = -scale;
	}
	//extract rotation
	//get mat upper left 3x3
	glm::mat3 rotation(from);
	//remove scaling
	if (scale.x)
		rotation[0] /= scale.x;
	if (scale.y)
		rotation[1] /= scale.y;
	if (scale.z)
		rotation[2] /= scale.z;
	glm::quat rotationQuat = glm::toQuat(rotation);

	to.setPosition(position);
	to.setRotation(rotationQuat);
	to.scale(scale.x, scale.y, scale.z);

	return to;
}

float radiansToDegrees(float radians)
{
	return (radians*180.0f) / PI;
}

float degreeToRadians(float degrees)
{
	return (degrees * PI) / 180.0f;
}

glm::quat eulerToQuaternion(glm::vec3 eulerAngles)
{
	glm::quat result;
	if (eulerAngles.x)
		result = glm::rotate(result, eulerAngles.x, glm::vec3(1.0f, 0, 0));
	if (eulerAngles.y)
		result = glm::rotate(result, eulerAngles.y, glm::vec3(0, 1.0f, 0));
	if (eulerAngles.z)
		result = glm::rotate(result, eulerAngles.z, glm::vec3(0, 0, 1.0f));
	return result;
}

glm::mat4 scalarMultiplication(float scalar, glm::mat4 matrix)
{
	glm::mat4 result = matrix;
	result[0][0] *= scalar; result[1][0] *= scalar;	result[2][0] *= scalar; result[3][0] *= scalar;
	result[0][1] *= scalar; result[1][1] *= scalar;	result[2][1] *= scalar; result[3][1] *= scalar;
	result[0][2] *= scalar; result[1][2] *= scalar;	result[2][2] *= scalar; result[3][2] *= scalar;
	result[0][3] *= scalar; result[1][3] *= scalar;	result[2][3] *= scalar; result[3][3] *= scalar;

	return result;
}

glm::vec3 dampen(glm::quat from, glm::vec3 dampVec)
{
	glm::vec3 euler = glm::degrees(glm::eulerAngles(from));
	if		(euler.x > 0 && euler.x > dampVec.x) euler.x = dampVec.x;
	else if (euler.x < -dampVec.x) euler.x = -dampVec.x;
	if		(euler.y > 0 && euler.y > dampVec.y) euler.y = dampVec.y;
	else if (euler.y < -dampVec.y) euler.y = -dampVec.y;
	if		(euler.z > 0 && euler.z > dampVec.z) euler.z = dampVec.z;
	else if (euler.z < -dampVec.z) euler.z = -dampVec.z;
	return euler;
}

float randomNumber(float Min, float Max)
{
    return ((float(rand()) / float(RAND_MAX)) * (Max - Min)) + Min;
}