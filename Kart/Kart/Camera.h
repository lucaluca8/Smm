#pragma once

#include<glad/glad.h>
#include<glfw3.h>
#include<glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <gtx/rotate_vector.hpp>
#include <gtx/vector_angle.hpp>
#include "ShaderClass.h"


class Camera
{
public:
	//store the main vectors of the camera
	glm::vec3 position;
	glm::vec3 orientation = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

	//boolean to prevent the camera from jumping when first clicking left click
	bool firstClick = true;

	//store the width and height of the window
	int width;
	int height;

	//adjust the speed fo the camera and it's sensitivity when looking around
	float speed = 0.1f;
	float sensitivity = 100.0f;

	Camera(int width, int height, glm::vec3 position);

	void Matrix(float FOVdeg, float nearPlane, float farPlane, ShaderClass& shader, const char* uniform);
	void Inputs(GLFWwindow* window);
};





