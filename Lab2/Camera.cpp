#pragma once
#include "Camera.h"

#include <vector>

#include <glm/gtc/matrix_transform.hpp>


Camera::Camera(const glm::vec3 &position/* = glm::vec3(0.0f, 0.0f, 0.0f)*/,
				const glm::vec3 &up /*= glm::vec3(0.0f, 1.0f, 0.0f)*/, 
				GLfloat yaw /*= -90.0f*/
				, GLfloat pitch /*= 0.0f*/)
	: Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(10.0f), MouseSensitivity(.025f), Zoom(45.0f),
	Position(position), WorldUp(up), Yaw(yaw), Pitch(pitch)
{
	this->updateCameraVectors();
}

Camera::Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat yaw, GLfloat pitch)
	: Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(10.0f), MouseSensitivity(.0025f), Zoom(45.0f),
	Position(glm::vec3(posX, posY, posZ)), WorldUp(glm::vec3(upX, upY, upZ)), Yaw(yaw), Pitch(pitch)
{
	this->updateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix() const
{
	return glm::lookAt(this->Position, this->Position + this->Front, this->WorldUp);
}

void Camera::ProcessKeyboard(Direction direction, GLfloat deltaTime)
{
	GLfloat velocity = this->MovementSpeed * deltaTime;
	if (direction == Direction::FORWARD)
		this->Position += this->Front * velocity;
	if (direction == Direction::BACKWARD)
		this->Position -= this->Front * velocity;
	if (direction == Direction::LEFT)
		this->Position -= this->Right * velocity;
	if (direction == Direction::RIGHT)
		this->Position += this->Right * velocity;
}

void Camera::ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch /*= true*/)
{
	xoffset *= this->MouseSensitivity;
	yoffset *= this->MouseSensitivity;



	this->Yaw += xoffset;
	this->Pitch += yoffset;

	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	if (constrainPitch)
	{
		if (this->Pitch > 89.0f)
			this->Pitch = 89.0f;
		if (this->Pitch < -89.0f)
			this->Pitch = -89.0f;
	}

	// Update Front, Right and Up Vectors using the updated Eular angles
	this->updateCameraVectors();
}

void Camera::ProcessMouseScroll(GLfloat yoffset)
{
	if (this->Zoom >= 1.0f && this->Zoom <= 45.0f)
		this->Zoom -= yoffset;
	if (this->Zoom <= 1.0f)
		this->Zoom = 1.0f;
	if (this->Zoom >= 45.0f)
		this->Zoom = 45.0f;
}

void Camera::updateCameraVectors()
{
	// Calculate the new Front vector
	glm::vec3 front;
	front.x = cos(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
	front.y = sin(glm::radians(this->Pitch));
	front.z = sin(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
	this->Front = glm::normalize(front);
	// Also re-calculate the Right and Up vector
	this->Right = glm::normalize(glm::cross(this->Front, this->WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	this->Up = glm::normalize(glm::cross(this->Front, this->Right));
}
