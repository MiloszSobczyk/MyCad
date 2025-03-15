#pragma once

#include "Algebra.h"

using namespace Algebra;

enum Camera_Movement 
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;


class Camera
{
public:
    // camera Attributes
    Vector4 Position;
    Vector4 Front;
    Vector4 Up;
    Vector4 Right;
    Vector4 WorldUp;
    // euler Angles
    float Yaw;
    float Pitch;
    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    // constructor with vectors
    Camera(Vector4 position = Vector4(0.0f, 0.0f, 0.0f), Vector4 up = Vector4(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) 
        : Front(Vector4(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }
    // constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) \
        : Front(Vector4(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = Vector4(posX, posY, posZ);
        WorldUp = Vector4(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    Algebra::Matrix4 GetViewMatrix()
    {
        Algebra::Matrix4 result(
            Right,
            Up,
            Front,
            Vector4(0.f, 0.f, 0.f, 1.f)
        );

        result = result * Algebra::Matrix4::Translation(-Position.x, -Position.y, -Position.z);

        return result;
    }

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD)
            Position = Position + Front * velocity;
        if (direction == BACKWARD)
            Position = Position - Front * velocity;
        if (direction == LEFT)
            Position = Position - Right * velocity;
        if (direction == RIGHT)
            Position = Position + Right * velocity;
    }

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw += xoffset;
        Pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        // update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset)
    {
        Zoom -= (float)yoffset;
        if (Zoom < 1.0f)
            Zoom = 1.0f;
        if (Zoom > 45.0f)
            Zoom = 45.0f;
    }

private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
        // calculate the new Front vector
        Vector4 front;
        front.x = cos(DegreeToRadians(Yaw)) * cos(DegreeToRadians(Pitch));
        front.y = sin(DegreeToRadians(Pitch));
        front.z = sin(DegreeToRadians(Yaw)) * cos(DegreeToRadians(Pitch));
        Front = front.Normalize();
        // also re-calculate the Right and Up vector
        Right = Vector4::Cross(Front, WorldUp).Normalize();  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up = Vector4::Cross(Right, Front).Normalize();
    }
};