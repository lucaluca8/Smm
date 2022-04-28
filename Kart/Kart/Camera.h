#pragma once

#include <glad/glad.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include <iostream>
#include <vector>

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

// default values
const float YAW = 0.0f;
const float PITCH = 0.0f;
const float SPEED = 20.0f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 80.0f;
const float ZOOM_RANGE = 20.0f;
const float ZOOM_SPEED = 2.0f;
const float STADIA = 1000.0f;

class Camera
{
public:
    // camera properties
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // angles
    float Yaw; // yaw angle
    float Pitch; // tilt angle
    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;
    // constructor
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);

    // calculate the view matrix
    glm::mat4 GetViewMatrix();
    // calculate the projection matrix
    glm::mat4 GetProjMatrix(float aspect);
    //method to process mouse movements
    void ProcessMouseMovement(float xoffset, float yoffset);
    //method to process keyboard input
    void ProcessKeyboard(Camera_Movement direction, float deltaTime);

    void updateCameraVectors();
};

