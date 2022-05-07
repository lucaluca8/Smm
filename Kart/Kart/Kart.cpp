#include "Kart.h"
#include <iostream>

Kart::Kart(glm::vec3 position) : MovementSpeed(40.0f)
, TurningSpeed(60.0f)
, Yaw(0.0f)
{
	Position = position;
    WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	updateFront();
}

glm::vec3 Kart::getPosition()
{
    glm::vec3 position = Position;
    position.x /= 2.0f;
    position.y += 3;
    position.z /= 2.0f;
    return position;
}

float Kart::getMidValYaw()
{
    return  Yaw/ 2;
}

glm::vec3 Kart::getMidValPosition()
{
    return Position / 2.0f;
}


float Kart::getYaw()
{
    return Yaw;
}

void Kart::ProcessKeyboard(Direction direction, float deltaTime)
{
    if (direction == K_FORWARD && Position.x>-594)
    {
        Yaw = 0.0f;
        Position -= Right * MovementSpeed * deltaTime;
    }
    if (direction == K_BACKWARD && Position.x<592)
    {
        Yaw = -120.0f;
        Position += Right * MovementSpeed * deltaTime;
    }
    if (direction == K_RIGHT && Position.z>-582)
    {
        Yaw = -60.0f;
        //Position += Front * MovementSpeed * deltaTime;
        Position += Front * MovementSpeed * deltaTime;
    }
    if (direction == K_LEFT && Position.z < 596)
    {
        Yaw = 60.0f;
        //Position -= Front * MovementSpeed * deltaTime;
        Position -= Front * MovementSpeed * deltaTime;
    }

}

void Kart:: updateFront()
{
    glm::vec3 front;
    front.x = -sin(glm::radians(getMidValYaw()));
    front.y = 0.0f;
    front.z = -cos(glm::radians(getMidValYaw()));
    Front = glm::normalize(front);

    Right = glm::normalize(glm::cross(Front, WorldUp));
}


