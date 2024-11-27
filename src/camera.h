#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <iostream>
#include <glm/glm.hpp>

enum Camera_Movement {
  FORWARD,
  BACKWARD,
  LEFT,
  RIGHT,
  UP,
  DOWN
};

class Camera {
  public:
    Camera(glm::vec3 position, glm::vec3 target) {
      position_ = position;
      auto front = target - position;

      front_ = glm::normalize(front);
      right_ = glm::normalize(glm::cross(front_, worldUp_));
      up_ = glm::normalize(glm::cross(right_, front_));

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
        if (direction == UP)
          position_ += up_ * velocity;
        if (direction == DOWN)
          position_ -= up_ * velocity;
    }
  void ProcessMouseMovement(double xoffset, double yoffset)
    {
      auto x_offset = static_cast<float>(xoffset * mouseSensitivity);
      auto y_offset = static_cast<float>(yoffset * mouseSensitivity);

      auto target = position_ + front_;
      target -= right_ * x_offset;
      target -= up_ * y_offset;

      glm::vec3 front = target - position_;
      front_ = glm::normalize(front);
      right_ = glm::normalize(glm::cross(front_, worldUp_));
      up_ = glm::normalize(glm::cross(right_, front_));

    }
    glm::mat4 GetViewMatrix()
    {
      return glm::lookAt(position_, position_ + front_, worldUp_);
    }
    float getZoom() const {
      return zoom;
    }
  private:
    glm::vec3 position_;

    glm::vec3 front_;
    glm::vec3 up_;
    glm::vec3 right_;

    glm::vec3 worldUp_ = glm::vec3(0.0f, 0.0f, 1.0f);
    double movementSpeed = 30.0;
    double mouseSensitivity = 0.005;
    float zoom = 45.0f;
};

#endif //CAMERA_H
