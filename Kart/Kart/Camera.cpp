#include "Camera.h"

Camera::Camera(int width, int height, glm::vec3 position)
{
	this->width = width;
	this->height = height;
	this->position = position;
}

void Camera::Matrix(float FOVdeg, float nearPlane, float farPlane, ShaderClass& shader, const char* uniform)
{
	//initializez matrices so they are not null
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	//make the camera look in the right direction from the right position
	view = glm::lookAt(position, position + orientation, up);
	//add perspective to the scene
	projection = glm::perspective(glm::radians(FOVdeg), (float)(width / height), nearPlane, farPlane);

	//export camera metrix to the vertex shader
	glUniformMatrix4fv(glGetUniformLocation(shader.id, uniform), 1, GL_FALSE, glm::value_ptr(projection * view));
}

//method that gets the keyboard inputs and uses them on the camera
void Camera::Inputs(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		position += speed*orientation;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		position += speed * -glm::normalize(glm::cross(orientation,up));
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		position += speed * -orientation;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		position += speed * glm::normalize(glm::cross(orientation, up));
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		position += speed * up;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		position += speed * -up;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		speed = 0.4f;
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
	{
		speed = 0.1f;
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		//hide mouse cursour
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		//prevent the camera from jumping on the first click
		if (firstClick)
		{
			glfwSetCursorPos(window, (width/2), (height/2));
			firstClick = false;
		}
		//save the coordonates of the curour
		double mouseX;
		double mouseY;
		//get the actual cursour coordinates
		glfwGetCursorPos(window, &mouseX, &mouseY);

		//make the cursor coordinates begin in the middle of the screen
		float rotx = sensitivity * (float)(mouseY - (height / 2)) / height;
		float roty = sensitivity * (float)(mouseX - (height / 2)) / height;

		//calculates upcoming vertical change in the orientation
		glm::vec3 newOrientation = glm::rotate(orientation, glm::radians(-rotx), glm::normalize(glm::cross(orientation, up)));

		//decide wheter or not the next vertical orientation is legal or not
		if (abs(glm::angle(newOrientation, up) - glm::radians(90.0f)) <= glm::radians(85.0f))
		{
			orientation = newOrientation;
		}

		// Rotates the Orientation left and right
		orientation = glm::rotate(orientation, glm::radians(-roty), up);

		// Sets mouse cursor to the middle of the screen so that it doesn't end up roaming around
		glfwSetCursorPos(window, (width / 2), (height / 2));
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	{
		//unhide the cursors because the camera will not work anymore
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

		//make sure the next time the camera is active it will not jump
		firstClick = true;
	}
}
