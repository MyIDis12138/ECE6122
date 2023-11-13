#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

// Default camera values
const float YAW         =  0.2*3.14f;
const float PITCH       =  0.5*3.14f;
const float ZOOM        =  45.0f;
const float RADIUS      =  40.0f;
const float MOVEMENT_SPEED = 2.0f;
const float ROTATION_SPEED = 0.1 * 3.14f;

glm::vec3 origin = glm::vec3(0.0f, 10.0f, 3.5f);

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // camera Attributes
    glm::vec3 Position;
    glm::vec3 Direction;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // euler Angles
    float Yaw;
    float Pitch;
    // camera options
    float MovementSpeed;
    float RotationSpeed;
    float Zoom;
    float Radius;

    // constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH): MovementSpeed(MOVEMENT_SPEED), RotationSpeed(ROTATION_SPEED), Zoom(ZOOM)
    {
        Position = position;
        Radius = glm::distance(Position, origin);
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position + Direction, origin, WorldUp);
    }

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime, GLboolean constrainYaw = true)
    {
        if (direction == FORWARD)
            Radius -= deltaTime * MovementSpeed;
        if (direction == BACKWARD)
            Radius += deltaTime * MovementSpeed;
        if (direction == LEFT)
            Pitch -= deltaTime * RotationSpeed;
        if (direction == RIGHT)
            Pitch += deltaTime * RotationSpeed;
        if (direction == UP)
            Yaw += deltaTime * RotationSpeed;
        if (direction == DOWN)
            Yaw -= deltaTime * RotationSpeed;
        if (constrainYaw)
            Yaw = glm::clamp(Yaw, -1.57f, 1.57f);
        updateCameraVectors();
    }

private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
        // calculate the new Front vector
        glm::vec3 direction(
            cos(Yaw) * sin(Pitch), 
            sin(Yaw),
            cos(Yaw) * cos(Pitch)
        );
        Direction = direction;
        // also re-calculate the Right and Up vector
        // Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        // Up    = glm::normalize(glm::cross(Right, Front));

        Position = Radius * direction;
    }
};
#endif
