#ifndef CAMERA_H
#define CAMERA_H

#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>

#include <vector>

// Set of directions in which the camera can move in space (separate from orientation)
enum CameraDirections {
    NONE = 0, FORWARD = 1, BACKWARD = 2, LEFT = 4, RIGHT = 8, UP = 16, DOWN = 32
};

// Camera default orientation
#define YAW -90.0f
#define PITCH 0.0f
#define SPEED 10.0f
#define SENSITIVITY 0.1f
#define ZOOM 45.0f
#define ZOOMSENSITIVITY 1.0f

/**
 * @brief Abstract camera class, with update functions for keyboard, mouse, and scroll wheel
 */
class Camera {
public:
    glm::vec3 position; // Camera position in world
    glm::vec3 front;    // Vector direction that the camera is facing
    glm::vec3 up;       // Upwards orientation directional vector from the center of the camera pane to the top edge of the camera pane
    glm::vec3 right;    // Rightwards orientation directional vector from the center of the camera pane to the right edge of the camera pane
    glm::vec3 worldUp;  // Global upwards direction

    float yaw, pitch;
    float movementSpeed, mouseSens, zoom, zoomSens;

    /**
     * @brief Construct a new Camera object
     *
     * @param position A glm::vec3, position of the camera. Defaults to (0, 0, 0)
     * @param worldUp A glm::vec3, world up vector. Defaults to (0, 1, 0)
     * @param yaw Yaw Euler orientation of camera. Defaults to -90
     * @param pitch Pitch Euler orientation of camera. Defaults to 0
     */
    Camera(glm::vec3 position = glm::vec3(0, 0, 0), glm::vec3 worldUp = glm::vec3(0, 1, 0), float yaw = YAW, float pitch = PITCH) : front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSens(SENSITIVITY), zoom(ZOOM), zoomSens(ZOOMSENSITIVITY) {
        this->position = position;
        this->worldUp = worldUp;
        this->yaw = yaw;
        this->pitch = pitch;
        updateVectors();
    }

    /**
     * @brief Get the View Matrix of current camera (useful for setting gl context view matrix)
     *
     * @return glm::mat4 view matrix
     */
    glm::mat4 getViewMatrix() {
        // lookAt (eye, center, up)
        // eye - position of camera
        // center - position that the camera is looking at
        // up - normalized up vector of how the camera is oriented
        return glm::lookAt(position, position + front, up);
    }

    /**
     * @brief Updates camera position based off of processed keyboard input. Accepts bitwise OR'd directions. Final direction is normalized to ensure constant velocity in each direction.
     *
     * @param direction Any combination of any enum value in CameraDirections (of FORWARD | BACKWARD | LEFT | RIGHT | UP | DOWN)
     * @param dT Time between frames (used to maintain smoother motion)
     */
    void updateKeyboard(int direction, float dT) {
        float vel = movementSpeed * dT;
        glm::vec3 update = glm::vec3(0, 0, 0);

        if (direction & FORWARD)
            update += front;
        if (direction & BACKWARD)
            update -= front;
        if (direction & LEFT)
            update -= right;
        if (direction & RIGHT)
            update += right;
        if (direction & UP)
            update += worldUp;
        if (direction & DOWN)
            update -= worldUp;

        if (glm::dot(update, update) != 0)
            position += glm::normalize(update) * vel;
    }

    /**
     * @brief Updates camera orientation based off of processed mouse input. Optional pitch constraint
     *
     * @param xoffset Mouse offset in x direction
     * @param yoffset Mouse offset in y direction
     * @param constrainPitch Whether or not the pitch should be constrained. Defaults to true
     */
    void updateMouse(float xoffset, float yoffset, bool constrainPitch = true) {
        xoffset *= mouseSens;
        yoffset *= mouseSens;

        yaw += xoffset;
        pitch += yoffset;

        // pitch binding so screen does not flip behave weirdly at edges
        if (constrainPitch) {
            if (pitch > 89.0f)
                pitch = 89.0f;
            else if (pitch < -89.0f)
                pitch = -89.0f;
        }

        // update by euler angles
        updateVectors();
    }

    /**
     * @brief Updates camera FOV based off of processed scroll wheel input
     *
     * @param offset How much the scroll wheel has been offset (may require scaling via zoomSens)
     */
    void updateScroll(float offset) {
        // TODO: implement fov control by zoom scalar
        zoom -= offset * zoomSens;
        if (zoom < 1.0f)
            zoom = 1.0f;
        if (zoom > 45.0f)
            zoom = 45.0f;
    }

protected:
    /**
     * @brief Updates camera directional vectors
     */
    void updateVectors() {
        glm::vec3 temp;
        temp.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        temp.y = sin(glm::radians(pitch));
        temp.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(temp);

        right = glm::normalize(glm::cross(front, worldUp));
        up = glm::normalize(glm::cross(right, front));
    }
};

/**
 * @brief A class of camera that rotates around a single point
 */
class ArcCamera : public Camera {
public:
    glm::vec3 center;
    float radius;

    /**
     * @brief Construct a new Arc Camera object
     *
     * @param lookAt Position to look at and revolve around
     * @param radius Distance away from the center of rotation
     * @param yaw Yaw Euler orientation of camera. Defaults to -90
     * @param pitch Pitch Euler orientation of camera. Defaults to 0
     */
    ArcCamera(glm::vec3 lookAt, float radius, float yaw = YAW, float pitch = PITCH) : Camera() {
        this->center = lookAt;
        this->yaw = yaw;        // 0 -> 2PI
        this->pitch = pitch;    // 0 -> PI
        this->radius = radius;
        this->position = computePosition();
        this->worldUp = glm::vec3(0, 1, 0);
        updateVectors();
    }

    /**
     * @brief Get the View Matrix of current camera (useful for setting gl context view matrix)
     *
     * @return glm::mat4 view matrix
     */
    glm::mat4 getViewMatrix() {
        // lookAt (eye, center, up)
        // eye - position of camera
        // center - position that the camera is looking at
        // up - normalized up vector of how the camera is oriented
        return glm::lookAt(position, center, up);
    }

    /**
     * @brief Updates camera position based off of processed keyboard input. Accepts bitwise OR'd directions
     *
     * @param direction Any combination of any enum value in CameraDirections (of FORWARD | BACKWARD | LEFT | RIGHT [UP and DOWN are ignored])
     * @param dT Time between frames (used to maintain smoother motion)
     */
    void updateKeyboard(int direction, float dT) {
        float vel = movementSpeed * dT;
        glm::vec2 update = glm::vec2(0, 0);

        if (direction & FORWARD)
            update.x += 1;
        if (direction & BACKWARD)
            update.x -= 1;
        if (direction & LEFT)
            update.y -= 1;
        if (direction & RIGHT)
            update.y += 1;

        if (glm::dot(update, update) != 0) {
            glm::vec2 temp = glm::normalize(update) * vel;
            pitch += temp.x;
            yaw += temp.y;

            // pitch binding so screen does not flip behave weirdly at edges
            if (pitch > 179.0f)
                pitch = 179.0f;
            else if (pitch < 1.0f)
                pitch = 1.0f;

            position = computePosition();
            updateVectors();
        }
    }

    /**
     * @brief Updates camera orientation based off of processed mouse input. Optional pitch constraint
     *
     * @param xoffset Mouse offset in x direction
     * @param yoffset Mouse offset in y direction
     * @param constrainPitch Whether or not the pitch should be constrained. Defaults to true
     */
    void updateMouse(float xoffset, float yoffset, bool constrainPitch = true) {
        xoffset *= mouseSens;
        yoffset *= mouseSens;

        yaw += xoffset;
        pitch += yoffset;

        // pitch binding so screen does not flip behave weirdly at edges
        if (constrainPitch) {
            if (pitch > 179.0f)
                pitch = 179.0f;
            else if (pitch < 1.0f)
                pitch = 1.0f;
        }

        // update by euler angles
        position = computePosition();
        updateVectors();
    }

    /**
     * @brief Updates radius based off of processed scroll wheel input
     *
     * @param offset How much the scroll wheel has been offset (may require scaling via zoomSens)
     */
    void updateScroll(float offset) {
        radius -= offset * zoomSens;
        if (radius < minimumScroll)
            radius = minimumScroll;
        else if (radius > maximumScroll)
            radius = maximumScroll;
    }

    float minimumScroll = 1.0f;
    float maximumScroll = 45.0f;

private:
    glm::vec3 computePosition() {
        float p = glm::radians(pitch);
        float y = glm::radians(yaw);
        return glm::vec3(sin(p) * cos(y), cos(p), sin(p) * sin(y)) * radius + center;
    }

    /**
     * @brief Updates camera directional vectors
     */
    void updateVectors() {
        front = glm::normalize(center - position);

        right = glm::normalize(glm::cross(front, worldUp));
        up = glm::normalize(glm::cross(right, front));
    }
};

#endif