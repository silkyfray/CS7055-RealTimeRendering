#include "SQTTransform.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/matrix_interpolation.hpp>

SQTTransform::SQTTransform(const glm::vec3 &initPosition,
	const glm::vec3 &initScale,
	const glm::vec3 &initRotation)
	:m_Position(initPosition), m_Scale(initScale)
{
	pivotOnLocalAxis(initRotation.x, initRotation.y, initRotation.z);
}

SQTTransform::SQTTransform(const glm::vec3 &initPosition,
	const glm::vec3 &initScale,
	const glm::quat &initRotation)
	:m_Position(initPosition), m_Scale(initScale), m_OrientationLocal(initRotation)
{

}

SQTTransform::SQTTransform()
	:m_Position(glm::vec3()), m_Scale(glm::vec3(1)), m_OrientationLocal(glm::quat())
{

}

void SQTTransform::setPosition(float x, float y, float z)
{
	m_Position.x = x;
	m_Position.y = y;
	m_Position.z = z;
}
void SQTTransform::setPosition(glm::vec3 position)
{
	m_Position = position;
}


void SQTTransform::setRotation(glm::quat rotation)
{
	m_OrientationLocal = rotation;
}
void SQTTransform::setRotation(glm::vec3 rotation)
{
	glm::quat result;
	if (rotation.x)
		result = glm::rotate(result, rotation.x, glm::vec3(1.0f, 0, 0));
	if (rotation.y)
		result = glm::rotate(result, rotation.y, glm::vec3(0, 1.0f, 0));
	if (rotation.z)
		result = glm::rotate(result, rotation.z, glm::vec3(0, 0, 1.0f));
	
	m_OrientationLocal = result;
	m_OrientationLocal = glm::normalize(m_OrientationLocal);
}

void SQTTransform::scaleUniform(float factor)
{
	m_Scale.x *= factor;
	m_Scale.y *= factor;
	m_Scale.z *= factor;
}

void SQTTransform::scale(float xFactor, float yFactor, float zFactor)
{
	m_Scale.x *= xFactor;
	m_Scale.y *= yFactor;
	m_Scale.z *= zFactor;
}

void SQTTransform::pivotOnLocalAxis(float x, float y, float z)
{
	if (x)
		m_OrientationLocal = glm::rotate(m_OrientationLocal, glm::degrees(x), glm::vec3(1.0f, 0, 0));
	if (y)
		m_OrientationLocal = glm::rotate(m_OrientationLocal, glm::degrees(y), glm::vec3(0, 1.0f, 0));
	if (z)
		m_OrientationLocal = glm::rotate(m_OrientationLocal, glm::degrees(z), glm::vec3(0, 0, 1.0f));
	m_OrientationLocal = glm::normalize(m_OrientationLocal);
}

void SQTTransform::pivotOnLocalAxisDegrees(float x, float y, float z)
{
	if (x)
		m_OrientationLocal = glm::rotate(m_OrientationLocal, x, glm::vec3(1.0f, 0, 0));
	if (y)
		m_OrientationLocal = glm::rotate(m_OrientationLocal, y, glm::vec3(0, 1.0f, 0));
	if (z)
		m_OrientationLocal = glm::rotate(m_OrientationLocal, z, glm::vec3(0, 0, 1.0f));
	m_OrientationLocal = glm::normalize(m_OrientationLocal);

}

void SQTTransform::pivotOnLocalAxis(glm::quat rotation)
{
	m_OrientationLocal *= rotation;
	m_OrientationLocal = glm::normalize(m_OrientationLocal);

}

void SQTTransform::rotateAroundGlobal(glm::quat rotation)
{
	m_Position = rotation * m_Position * glm::inverse(rotation);
}

void SQTTransform::rotateAroundGlobal(float x, float y, float z)
{
	glm::quat newRotation;
	if (x)
		newRotation = glm::rotate(glm::quat(), glm::degrees(x), glm::vec3(1.0f, 0, 0));
	if (y)
		newRotation = glm::rotate(newRotation, glm::degrees(y), glm::vec3(0, 1.0f, 0));
	if (z)
		newRotation = glm::rotate(newRotation, glm::degrees(z), glm::vec3(0, 0, 1.0f));
	newRotation = glm::normalize(newRotation);
	m_Position = newRotation * m_Position * glm::inverse(newRotation);

}

void SQTTransform::pivotOnGlobalAxis(glm::mat4 rotationMatrix)
{
	glm::mat4 oldRotation = glm::toMat4(m_OrientationLocal);

	m_OrientationLocal = glm::toQuat(glm::extractMatrixRotation(rotationMatrix)*oldRotation);
	//normalize so no build up of error
	m_OrientationLocal = glm::normalize(m_OrientationLocal);
}


void SQTTransform::pivotOnGlobalAxis(float x, float y, float z)
{
	glm::mat4 newRotation;
	if (x)
		newRotation = glm::rotate(newRotation, x, glm::vec3(1.0f, 0, 0));
	if (y)
		newRotation = glm::rotate(newRotation, y, glm::vec3(0, 1.0f, 0));
	if (z)
		newRotation = glm::rotate(newRotation, z, glm::vec3(0, 0, 1.0f));
	glm::mat4 oldRotation = glm::toMat4(m_OrientationLocal);

	m_OrientationLocal = glm::toQuat(glm::extractMatrixRotation(newRotation)*oldRotation);
	//normalize so no build up of error
	m_OrientationLocal = glm::normalize(m_OrientationLocal);
}

void SQTTransform::pivotOnGlobalAxis(glm::vec3 rotation)
{
	pivotOnGlobalAxis(rotation.x, rotation.y, rotation.z);
}


void SQTTransform::pivotOnAngleAxis(float angle, glm::vec3 axis)
{
	//REMINDER to myself: do not use quaternions as rotation axis. DO NOT. I know it will seem logical down the road but don't. It gives different results. Try it if you will
	glm::mat4 newRotation = glm::axisAngleMatrix(axis, angle);
	glm::mat4 oldRotation = glm::toMat4(m_OrientationLocal);

	m_OrientationLocal = glm::toQuat(glm::extractMatrixRotation(newRotation)*oldRotation);
	//normalize so no build up of error
	m_OrientationLocal = glm::normalize(m_OrientationLocal);
}
void SQTTransform::pivotOnAngleAxis(glm::quat rotation)
{
	//REMINDER to myself: do not use quaternions as rotation axis. DO NOT. I know it will seem logical down the road but don't. It gives different results. Try it if you will
	glm::mat4 newRotation = glm::toMat4(rotation);
	glm::mat4 oldRotation = glm::toMat4(m_OrientationLocal);

	m_OrientationLocal = glm::toQuat(glm::extractMatrixRotation(newRotation)*oldRotation);
	//normalize so no build up of error
	m_OrientationLocal = glm::normalize(m_OrientationLocal);
}


void SQTTransform::rotateAroundPivot(glm::quat rotation, glm::vec3 pivot)
{
	glm::vec3 dir = m_Position - pivot;
	dir = rotation * dir;
	m_Position = dir + pivot;

}
void SQTTransform::rotateAndPivotAroundPoint(glm::quat rotation, glm::vec3 pivot)
{
	rotateAroundPivot(rotation, pivot);

	m_OrientationLocal = rotation * m_OrientationLocal;
}
void SQTTransform::translateLocal(float x, float y, float z)
{
	glm::vec3 displacement = m_OrientationLocal * glm::vec3(x, y, z);
	m_Position += displacement;
}

void SQTTransform::translateLocal(glm::vec3 translation)
{
	glm::vec3 displacement = m_OrientationLocal * translation;
	m_Position += displacement;
}

void SQTTransform::translateOnAxis(float distance, glm::vec3 axis)
{
	m_Position += glm::normalize(axis)*distance;
}

void SQTTransform::translateGlobal(float x, float y, float z)
{
	m_Position.x += x;
	m_Position.y += y;
	m_Position.z += z;
}

glm::mat4 SQTTransform::getMatrix() const
{
	glm::mat4 modelMatrix;
	modelMatrix = glm::translate(modelMatrix, m_Position);
	modelMatrix = glm::scale(modelMatrix, m_Scale);
	modelMatrix = modelMatrix * glm::toMat4(m_OrientationLocal); // rotate local
	return modelMatrix;
};

glm::vec3 SQTTransform::getPosition() const
{
	return m_Position;
}
glm::vec3 SQTTransform::getScale() const
{
	return m_Scale;
}
glm::quat SQTTransform::getOrientation() const
{
	return m_OrientationLocal;
}

glm::vec3 SQTTransform::getForwardDirection() const
{
	return glm::normalize(m_OrientationLocal * glm::vec3(0, 0, 1));
}
glm::vec3 SQTTransform::getRightDirection() const
{
	return glm::normalize(m_OrientationLocal * glm::vec3(1,0, 0));
}
glm::vec3 SQTTransform::getUpDirection() const
{
	return glm::normalize(m_OrientationLocal * glm::vec3(0, 1, 0));
}

SQTTransform SQTTransform::interpolate(const SQTTransform &other, float blendFactor)
{
	glm::vec3 position =  (1-blendFactor) * m_Position + blendFactor * other.getPosition();
	glm::vec3 scale =  (1-blendFactor) * m_Scale + blendFactor * other.getScale();
	glm::quat rotation = glm::slerp(m_OrientationLocal,other.getOrientation(), blendFactor) ;
	return SQTTransform(position, scale, rotation); 
}
