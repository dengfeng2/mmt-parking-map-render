#include "gl_util.h"


const GLchar *vertexShaderSource = R"(#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;

out vec4 vertexColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    vertexColor = aColor;
})";
const GLchar *fragmentShaderSource = R"(#version 330 core
in vec4 vertexColor;
out vec4 FragColor;

uniform vec4 mainColor;

void main()
{
    FragColor = vertexColor;
})";


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        auto* context = static_cast<MouseContext*>(glfwGetWindowUserPointer(window));
        if (action == GLFW_PRESS) {
            if (context) {
                context->leftMouseButton = true;
                double xpos, ypos;
                glfwGetCursorPos(window, &xpos, &ypos);
                context->lastX = xpos;
                context->lastY = ypos;
            }
        } else if (action == GLFW_RELEASE) {
            if (context) {
                context->leftMouseButton = false;
            }
        }
    }
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    auto* context = static_cast<MouseContext*>(glfwGetWindowUserPointer(window));
    if (context != nullptr && context->leftMouseButton) {

        double xoffset = xposIn - context->lastX;
        double yoffset = context->lastY - yposIn; // reversed since y-coordinates go from bottom to top

        context->lastX = xposIn;
        context->lastY = yposIn;


        context->camera.ProcessMouseMovement(xoffset, yoffset);
    }
}

bool GLUtil::init(glm::vec3 position, glm::vec3 target) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    mouse_context_ = std::make_unique<MouseContext>(Camera(position, target));

    window_ = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window_ == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(window_);
    glfwSetFramebufferSizeCallback(window_, [](GLFWwindow* window, int width, int height){glViewport(0, 0, width, height);});
    glfwSetWindowUserPointer(window_, mouse_context_.get());
    glfwSetMouseButtonCallback(window_, mouse_button_callback);
    glfwSetCursorPosCallback(window_, mouse_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return false;
    }
    glEnable(GL_DEPTH_TEST);
    shader_ = std::make_unique<Shader>(vertexShaderSource, fragmentShaderSource);
    shader_->use();
    shader_->setVec4("mainColor", 1.0f, 1.0f, 0.0f, 1.0f);

    inited = true;
    return true;
}

void GLUtil::processInput(float deltaTime) {
    if (!inited) {
        std::cerr << "Failed to initialize OpenGL context" << std::endl;
        return;
    }
    if(glfwGetKey(window_, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window_, true);

    if (glfwGetKey(window_, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || glfwGetKey(window_, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS) {
        deltaTime = deltaTime * 0.1f;
    }
    if (glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS)
        mouse_context_->camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS)
        mouse_context_->camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS)
        mouse_context_->camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS)
        mouse_context_->camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window_, GLFW_KEY_E) == GLFW_PRESS)
        mouse_context_->camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window_, GLFW_KEY_C) == GLFW_PRESS)
        mouse_context_->camera.ProcessKeyboard(DOWN, deltaTime);
}

void GLUtil::updateTransforms() {
    if (!inited) {
        std::cerr << "Failed to initialize OpenGL context" << std::endl;
        return;
    }

    // render the triangle
    shader_->use();

    // pass projection matrix to shader (note that in this case it could change every frame)
    glm::mat4 projection = glm::perspective(glm::radians(mouse_context_->camera.getZoom()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 500.0f);
    shader_->setMat4("projection", projection);

    // camera/view transformation
    glm::mat4 view = mouse_context_->camera.GetViewMatrix();
    shader_->setMat4("view", view);

    glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    shader_->setMat4("model", model);
}













