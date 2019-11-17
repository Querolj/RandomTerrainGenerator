#include "Camera.h"
#include "ShaderManager.h"

Camera::Camera(glm::vec3 cam_position,
	glm::vec3 cam_front,
	float yaw, float pitch, float fov
)
{
	this->_cam_position = cam_position;
	this->_cam_front = cam_front;
	this->_world_up = glm::vec3(0.f, 1.f, 0.f);
	this->_yaw = yaw;
	this->_pitch = pitch;
	this->_fov = fov;

	ViewMatrix = glm::mat4x4(1.f);
	ViewMatrix = glm::lookAt(cam_position, cam_position + cam_front, _world_up);


}


void Camera::UpdateCamera(ShaderManager& shader_manager, int frame_buffer_width, int frame_buffer_height)
{
	if (_yaw != _old_yaw || _pitch != _old_pitch)
	{
		glm::vec3 front;
		front.x = cos(glm::radians(_pitch)) * cos(glm::radians(_yaw));
		front.y = sin(glm::radians(_pitch));
		front.z = cos(glm::radians(_pitch)) * sin(glm::radians(_yaw));
		_cam_front = glm::normalize(front);
	}

	ViewMatrix = glm::lookAt(_cam_position, _cam_position + _cam_front, _world_up);
	string matrix_name = "ViewMatrix";
	shader_manager.setMat4(matrix_name, ViewMatrix);
	//Set PM
	ProjectionMatrix = glm::mat4(1.f);
	ProjectionMatrix = glm::perspective(
		glm::radians(GetFov()),
		static_cast<float>(frame_buffer_width) / frame_buffer_height,
		_near_plane,
		_far_plane
	);
	matrix_name = "ProjectionMatrix";
	shader_manager.setMat4(matrix_name, ProjectionMatrix);


	_old_yaw = _yaw;
	_old_pitch = _pitch;

}
void Camera::CameraMovement(Camera_Movement move_type, float speed)
{
	switch (move_type)
	{
	case FORWARD:
		_cam_position += glm::normalize(_cam_front) * speed;
		break;
	case BACKWARD:
		_cam_position -= glm::normalize(_cam_front) * speed;
		break;
	case LEFT:
		_cam_position -= glm::normalize(glm::cross(_cam_front, _world_up)) * speed;
		break;
	case RIGHT:
		_cam_position += glm::normalize(glm::cross(_cam_front, _world_up)) * speed;
		break;
	case UP:
		_cam_position.y += speed;
		break;
	case DOWN:
		_cam_position.y -= speed;
		break;
	default:
		break;
	}
}

void Camera::UpdateFov(float yoffset)
{
	if (_fov >= 1.0f && _fov <= 90.0f)
	{
		_fov -= yoffset;
	}
	if (_fov <= 1.0f)
	{
		_fov = 1.0f;
	}
	if (_fov >= 90.0f)
	{
		_fov = 90.0f;
	}
}

void Camera::UpdateMouseRotation(float x_offset, float y_offset)
{
	_yaw += x_offset;
	_pitch += y_offset;

	if (_pitch > 89.0f)
		_pitch = 89.0f;
	if (_pitch < -89.0f)
		_pitch = -89.0f;
}

glm::vec3 Camera::GetCameraPosition() const
{
	return _cam_position;
}

glm::vec3 Camera::GetCameraFront() const
{
	return _cam_front;
}

glm::mat4x4 Camera::GetViewMatrix() const
{
	return ViewMatrix;
}

glm::mat4x4 Camera::GetProjectionMatrix() const
{
	return ProjectionMatrix;
}

glm::float32 Camera::GetFov() const
{
	return _fov;
}

void Camera::SetCameraPosition(glm::vec3 new_position)
{
	_cam_position = new_position;
}

void Camera::SetSpeed(float speed)
{
	this->_speed = speed;
}

float Camera::GetSpeed()
{
	return _speed;
}

glm::vec3 Camera::GetCameraDirection() const
{
	return glm::normalize(_cam_position - _cam_front);
}

