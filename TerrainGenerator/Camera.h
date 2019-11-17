#pragma once
#include "VerticesStruct.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdio.h>

#include "ShaderManager.h"



// Defines several possible options for camera movement.
// Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};


class Camera
{
private:
	glm::vec3 _world_up;
	glm::vec3 _cam_front;
	glm::vec3 _cam_position;

	float _fov = 90.f;
	float _yaw = -90.f;
	float _old_yaw;
	float _pitch = 0.f;
	float _old_pitch;

	//FPS mode variables
	float _speed = 1.f;

	//Projection & view matrix
	glm::mat4 ProjectionMatrix;
	glm::mat4 ViewMatrix;

	//Camera plane distances
	float _near_plane = 0.1f;
	float _far_plane = 1000.f;


public:
	Camera(glm::vec3 cam_position = glm::vec3(0.0f, 0.0f, 2.0f),
		glm::vec3 cam_front = glm::vec3(0.f, 0.f, 1.f),
		float yaw = -90.f, float pitch = 0.f, float fov = 90.f);

	//Send view_matrix to the shaders
	void UpdateCamera(ShaderManager& shader_manager, int frame_buffer_width, int frame_buffer_height);
	//Camera movement
	void CameraMovement(Camera_Movement move_type, float speed);
	//Update fov according to input mouse scroll 
	void UpdateFov(float yoffset);
	//Update camera rotation according to mouse position
	void UpdateMouseRotation(float x_offset, float y_offset);

	//Get camera data
	glm::vec3 GetCameraPosition() const;
	glm::vec3 GetCameraFront() const;
	glm::vec3 GetCameraDirection() const;
	glm::mat4 GetViewMatrix() const;
	glm::mat4 GetProjectionMatrix() const;
	glm::float32 GetFov() const;

	//Set camera position
	void SetCameraPosition(glm::vec3 new_position);

	//Fps mode
	void SetSpeed(float speed);
	float GetSpeed();

};
