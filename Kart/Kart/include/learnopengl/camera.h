#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <vector>

// enum for camera movement
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

// process input and compute the corespoding euler angles,vector and matrices for use in opengl
class Camera {
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
    Camera(
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
        float yaw = YAW,
        float pitch = PITCH)
        : Front(glm::vec3(0.0f, 0.0f, -1.0f))
        , MovementSpeed(SPEED)
        , MouseSensitivity(SENSITIVITY)
        , Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    // constructor withscalar value
    Camera(
        float posX, float posY, float posZ,
        float upX, float upY, float upZ,
        float yaw,
        float pitch)
        : Front(glm::vec3(0.0f, 0.0f, -1.0f))
        , MovementSpeed(SPEED)
        , MouseSensitivity(SENSITIVITY)
        , Zoom(ZOOM)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    // calculate the view matrix
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    // calculate the projection matrix
    glm::mat4 GetProjMatrix(float aspect)
    {
        return glm::perspective(glm::radians(Zoom), aspect, 0.1f, STADIA);
    }

    // process keyboard input
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD)
            Position += Front * velocity;
        if (direction == BACKWARD)
            Position -= Front * velocity;
        if (direction == LEFT)
            Position -= Right * velocity;
        if (direction == RIGHT)
            Position += Right * velocity;
        if (direction == UP)
            Position += WorldUp * velocity;
        if (direction == DOWN)
            Position -= WorldUp * velocity;
    }

    // process mouse movement
    void ProcessMouseMovement(float xoffset, float yoffset)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw -= xoffset;
        Pitch += yoffset;

        // limit the angle to avoid flipping issues
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;

        // update fron,up,right vectors using angles
        updateCameraVectors();
    }

    // process mouse wheele scrolling
    void ProcessMouseScroll(float yoffset)
    {
        if (Zoom >= 1.0f && Zoom <= ZOOM)
            Zoom -= yoffset;
        if (Zoom <= 1.0f)
            Zoom = 1.0f;
        if (Zoom >= ZOOM)
            Zoom = ZOOM;
    }

    void FixView(glm::vec3 position, float yaw)
    {
        Position = position;
        Yaw = yaw;
        Pitch = 0.0f;
        updateCameraVectors();
    }

    void ZoomIn()
    {
        if (Zoom >= ZOOM - ZOOM_RANGE)
            Zoom -= ZOOM_SPEED;
    }

    void ZoomOut()
    {
        if (Zoom <= ZOOM + ZOOM_RANGE)
            Zoom += ZOOM_SPEED;
    }

    void ZoomRecover()
    {
        if (Zoom < ZOOM)
            Zoom += ZOOM_SPEED / 2;
        if (Zoom > ZOOM)
            Zoom -= ZOOM_SPEED / 2;
    }

private:
    // calculate front,right,up vectors from the angles of the camera
    void updateCameraVectors()
    {
        // Calculate the new Front vector
        glm::vec3 front;
        front.x = -sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = -cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // recalculate the right and up vectors
        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up = glm::normalize(glm::cross(Right, Front));
    }
};
#endif