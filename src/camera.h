#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <iostream>
#include <glm/glm.hpp>

enum Camera_Movement {
  FORWARD,
  BACKWARD,
  LEFT,
  RIGHT
};

class Camera {
  public:
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), float yaw = -90.0f, float pitch = 0.0f) {
      position_ = position;
      yaw_ = yaw;
      pitch_ = pitch;
      updateCameraVectors();
    }

    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = movementSpeed * deltaTime;
        if (direction == FORWARD)
          position_ += front_ * velocity;
        if (direction == BACKWARD)
          position_ -= front_ * velocity;
        if (direction == LEFT)
          position_ -= right_ * velocity;
        if (direction == RIGHT)
          position_ += right_ * velocity;
    }
  void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
      xoffset *= movementSpeed;
      yoffset *= movementSpeed;

      yaw_   += xoffset;
      pitch_ += yoffset;

      // make sure that when pitch is out of bounds, screen doesn't get flipped
      if (constrainPitch)
      {
        if (pitch_ > 89.0f)
          pitch_ = 89.0f;
        if (pitch_ < -89.0f)
          pitch_ = -89.0f;
      }
      // update Front, Right and Up Vectors using the updated Euler angles
      updateCameraVectors();
    }
    glm::mat4 GetViewMatrix()
    {
      return glm::lookAt(position_, position_ + front_, up_);
    }
    float getZoom() const {
      return zoom;
    }
  private:
    void updateCameraVectors() {
      glm::vec3 front;
      front.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
      front.y = sin(glm::radians(pitch_));
      front.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
      front_ = glm::normalize(front);
      right_ = glm::normalize(glm::cross(front_, worldUp_));
      up_ = glm::normalize(glm::cross(right_, front_));
    }
    glm::vec3 position_;
    float yaw_;
    float pitch_;

    glm::vec3 front_;
    glm::vec3 up_;
    glm::vec3 right_;

    glm::vec3 worldUp_ = glm::vec3(0.0f, 1.0f, 0.0f);
    float movementSpeed = 7.0f;
    float mouseSensitivity = 0.05f;
    float zoom = 45.0f;
};

#endif //CAMERA_H
