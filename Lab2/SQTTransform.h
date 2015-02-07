#pragma once

//#include "stdafx.h"
#include <glm\glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

/**@brief Represents the transformations associated for a single Object instance.
@todo add getUp, getForward and getSide functions
@todo change Camera class to use SQTTransform
@todo add rotations around a point
*/
class SQTTransform
{
public:
	SQTTransform(const glm::vec3 &initPosition,
		const glm::vec3 &initScale,
		const glm::vec3 &initRotation);

	SQTTransform(const glm::vec3 &initPosition,
		const glm::vec3 &initScale,
		const glm::quat &initRotation);

	/** Identify transform*/
	SQTTransform();

	void setPosition(float x, float y, float z);
	void setPosition(glm::vec3 position);
	void setRotation(glm::quat rotation);
	void setRotation(glm::vec3 rotation);

	void scaleUniform(float factor);
	void scale(float xFactor, float yFactor, float zFactor);
	/**Orientates around the local axis*/
	void pivotOnLocalAxis(float x, float y, float z);
	void pivotOnLocalAxisDegrees(float x, float y, float z);
	/**Orientates around the local axis*/
	void pivotOnLocalAxis(glm::quat rotation);
	/**Rotates on the global axis without changing the orientation. Moves the position*/
	void rotateAroundGlobal(float x, float y, float z);
	/**Rotates on the global axis without changing the orientation. Moves the position*/
	void rotateAroundGlobal(glm::quat rotation);
	/**Rotate position around an artbitrary point without changing the orientation. */
	void rotateAroundPivot(glm::quat rotation, glm::vec3 pivot);
	/**Rotate position around an artbitrary point and as well as pivot on the local axis*/
	void rotateAndPivotAroundPoint(glm::quat rotation, glm::vec3 pivot);
	/**pivots using the global axis as the axis of rotation*/
	void pivotOnGlobalAxis(float x, float y, float z);
	/**pivots using the global axis as the axis of rotation*/
	void pivotOnGlobalAxis(glm::vec3 rotation);
	/**pivots using the global axis as the axis of rotation*/
	void pivotOnGlobalAxis(glm::mat4 rotationMatrix);
	/**pivots using an arbitrary axis of rotation as the axis of rotation*/
	void pivotOnAngleAxis(float angle, glm::vec3 axis);
	void pivotOnAngleAxis(glm::quat rotation);

	/**Translate in the direction of the local axis*/
	void translateLocal(float x, float y, float z);
	void translateLocal(glm::vec3 translation);
	/**Translate in the diretion of the global axis*/
	void translateGlobal(float x, float y, float z);
	/**Translate in the diretion of an arbitrary axis*/
	void translateOnAxis(float distance, glm::vec3 axis);

	SQTTransform interpolate(const SQTTransform &other, float blendFactor);

	/**Returns localOrientation * Scale * Translation */
	glm::mat4 getMatrix() const;

	glm::vec3 getPosition() const;
	glm::vec3 getScale() const;
	glm::quat getOrientation() const;

	glm::vec3 getForwardDirection() const;
	glm::vec3 getRightDirection() const;
	glm::vec3 getUpDirection() const;
private:
	glm::quat m_OrientationLocal;
	glm::vec3 m_Scale;
	glm::vec3 m_Position;
};