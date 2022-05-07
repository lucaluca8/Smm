#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <queue>

enum Direction {
    K_FORWARD,
    K_BACKWARD,
    K_LEFT,
    K_RIGHT
};

class Kart
{

public:
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Right;
    glm::vec3 Up;
    glm::vec3 WorldUp;
    float Yaw;

    float MovementSpeed;
    float TurningSpeed;

    Kart(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f));
    float getMidValYaw();
    glm::vec3 getMidValPosition();
    float getDelayYaw();
    float getYaw();
    void ProcessKeyboard(Direction direction, float deltaTime);
    glm::vec3 getPosition();
private:
    void updateFront();

};

