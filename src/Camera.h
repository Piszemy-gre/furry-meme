
#ifndef CAMERA_H_
#define CAMERA_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glfwpp/glfwpp.h>

class Camera
{
public:
    Camera()
    {
        update(0);
    }

    void onKeyEvent(glfw::KeyCode keyCode, int scanCode, glfw::KeyState keyState, glfw::ModifierKeyBit modifierKeyBit)
    {
        if (keyState == glfw::KeyState::Repeat)
            return;

        switch (keyCode)
        {
        case glfw::KeyCode::W:
            forward = (keyState == glfw::KeyState::Press);
            break;

        case glfw::KeyCode::S:
            backward = (keyState == glfw::KeyState::Press);
            break;

        case glfw::KeyCode::A:
            left = (keyState == glfw::KeyState::Press);
            break;

        case glfw::KeyCode::D:
            right = (keyState == glfw::KeyState::Press);
            break;

        case glfw::KeyCode::KeyPadAdd:
            zoomIn = (keyState == glfw::KeyState::Press);
            break;

        case glfw::KeyCode::KeyPadSubtract:
            zoomOut = (keyState == glfw::KeyState::Press);
            break;
        }
    }

    void onScrollEvent(float xOffset, float yOffset)
    {
        zoom += yOffset * zoomSensitivity;
    }

    void onCursorMoveEvent(float xOffset, float yOffset)
    {
        yaw += yOffset * mouseSensitivity;
        pitch -= xOffset * mouseSensitivity;
    }

    void update(float delta)
    {
        freeCamPositionUpdate(delta);

        const auto sinYaw = glm::sin(yaw);
        const auto cosYaw = glm::cos(yaw);
        const auto sinPitch = glm::sin(pitch);
        const auto cosPitch = glm::cos(pitch);

        frontDirection.z = sinYaw * cosPitch;
        frontDirection.x = sinYaw * sinPitch;
        frontDirection.y = cosYaw;
        frontDirection = glm::normalize(frontDirection);

        rightDirection.x = cosPitch;
        rightDirection.y = 0.f;
        rightDirection.z = -sinPitch;

        upDirection = glm::cross(frontDirection, rightDirection);

        V = glm::lookAt(position, position + frontDirection, glm::normalize(upDirection));
    }

    const glm::vec3 &getPosition() const
    {
        return position;
    }

    const glm::vec3 &getFrontDirection() const
    {
        return frontDirection;
    }

    const glm::mat4 &getV() const
    {
        return V;
    }

private:
    void freeCamPositionUpdate(float delta)
    {
        float distance = freecamSpeed * delta;

        if (forward)
            position += frontDirection * distance;
        if (backward)
            position -= frontDirection * distance;

        if (right)
            position -= rightDirection * distance;
        if (left)
            position += rightDirection * distance;

        if (zoomIn)
            position += zoomSensitivity * delta;
        if (zoomOut)
            position -= zoomSensitivity * delta;
    }

    glm::mat4 V;
    glm::vec3 position = glm::vec3(0.f, 1.f, 2.f), frontDirection, upDirection, rightDirection;
    float yaw = glm::pi<float>() / 2.f;
    float pitch = glm::pi<float>() / 4.f;
    float zoom = 1.f;
    bool forward = false;
    bool backward = false;
    bool left = false;
    bool right = false;
    bool zoomIn = false;
    bool zoomOut = false;
    static constexpr float freecamSpeed = 10.f;
    static constexpr float mouseSensitivity = 0.004f;
    static constexpr float zoomSensitivity = 0.5f;
};

#endif /* !CAMERA_H_ */
