#ifndef GL_UTIL_H
#define GL_UTIL_H
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <camera.h>
#include "shader.h"

const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 1200;

struct MouseContext {
    explicit MouseContext(Camera &&camera): camera(camera) {}
    Camera camera;
    double lastX = SCR_WIDTH / 2.0;
    double lastY = SCR_HEIGHT / 2.0f;
    bool leftMouseButton = false;
};

class GLUtil {
public:
    GLUtil() = default;
    bool init(glm::vec3 position, glm::vec3 target);
    void processInput(float deltaTime);
    void updateTransforms();

    GLFWwindow* window() {return window_;}

private:
    bool inited{false};
    GLFWwindow* window_ = nullptr;
    std::unique_ptr<MouseContext> mouse_context_ = nullptr;
    std::unique_ptr<Shader> shader_ = nullptr;
};




#endif //GL_UTIL_H
