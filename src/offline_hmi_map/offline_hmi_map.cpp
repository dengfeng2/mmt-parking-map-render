#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <fstream>

#include "shader.h"
#include "camera.h"
#include "hmi_map/hmi_map.h"

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow *window, float deltaTime, Camera &camera);

// settings
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 1200;

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

struct MouseContext {
    Camera camera;
    double lastX = SCR_WIDTH / 2.0;
    double lastY = SCR_HEIGHT / 2.0f;
    bool leftMouseButton = false;
};


struct FloorVAO {
    float floorName{};

    std::vector<unsigned int> pillarVAOs;
    std::vector<unsigned int> pillarVertexVBOs;
    std::vector<unsigned int> pillarColorVBOs;
    std::vector<unsigned int> pillarEBOs;

    std::vector<unsigned int> psdVAOs;
    std::vector<unsigned int> psdVertexVBOs;
    std::vector<unsigned int> psdColorVBOs;
    std::vector<unsigned int> psdEBOs;

    std::vector<unsigned int> speedBumpVAOs;
    std::vector<unsigned int> speedBumpVertexVBOs;
    std::vector<unsigned int> speedBumpColorVBOs;
    std::vector<unsigned int> speedBumpEBOs;

    std::vector<unsigned int> roadVAOs;
    std::vector<unsigned int> roadVertexVBOs;
    std::vector<unsigned int> roadColorVBOs;
    std::vector<unsigned int> roadEBOs;

    std::vector<unsigned int> roadPointNums;
};

int main(int argc, char *argv[])
{
    if (argc != 2) {
        cout << "Usage: ./offline_hmi_map <map_file>" << endl;
        return 1;
    }
    HMIMap hmi_map(argv[1]);
    auto startPoint = hmi_map.getStartPoint();
    auto endPoint = hmi_map.getEndPoint();

    MouseContext mouseContext = {Camera(glm::vec3(startPoint[0], startPoint[1], startPoint[2]+10),
        glm::vec3(endPoint[0], endPoint[1], endPoint[2]))};

    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetWindowUserPointer(window, &mouseContext);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    // tell GLFW to capture our mouse
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader program
    // ------------------------------------
    Shader ourShader(vertexShaderSource, fragmentShaderSource); // you can name your shader files however you like

    std::vector<FloorVAO> floorVAOs;
    auto floorNames = hmi_map.getFloorNames();
    for (auto floorName : floorNames) {
        FloorVAO floorVAO;
        floorVAO.floorName = floorName;

        size_t objNum{};

        objNum = hmi_map.getPillars(floorName).size();
        floorVAO.pillarVAOs.resize(objNum);
        floorVAO.pillarVertexVBOs.resize(objNum);
        floorVAO.pillarColorVBOs.resize(objNum);
        floorVAO.pillarEBOs.resize(objNum);
        glGenVertexArrays(objNum, floorVAO.pillarVAOs.data());
        glGenBuffers(objNum, floorVAO.pillarVertexVBOs.data());
        glGenBuffers(objNum, floorVAO.pillarColorVBOs.data());
        glGenBuffers(objNum, floorVAO.pillarEBOs.data());
        HMIMap::bindPillarsData(hmi_map, floorName,
            floorVAO.pillarVAOs, floorVAO.pillarVertexVBOs, floorVAO.pillarColorVBOs, floorVAO.pillarEBOs);

        objNum = hmi_map.getPsds(floorName).size();
        floorVAO.psdVAOs.resize(objNum);
        floorVAO.psdVertexVBOs.resize(objNum);
        floorVAO.psdColorVBOs.resize(objNum);
        floorVAO.psdEBOs.resize(objNum);
        glGenVertexArrays(objNum, floorVAO.psdVAOs.data());
        glGenBuffers(objNum, floorVAO.psdVertexVBOs.data());
        glGenBuffers(objNum, floorVAO.psdColorVBOs.data());
        glGenBuffers(objNum, floorVAO.psdEBOs.data());
        HMIMap::bindPsdsData(hmi_map, floorName,
            floorVAO.psdVAOs, floorVAO.psdVertexVBOs, floorVAO.psdColorVBOs, floorVAO.psdEBOs);

        objNum = hmi_map.getSpeedBumps(floorName).size();
        floorVAO.speedBumpVAOs.resize(objNum);
        floorVAO.speedBumpVertexVBOs.resize(objNum);
        floorVAO.speedBumpColorVBOs.resize(objNum);
        floorVAO.speedBumpEBOs.resize(objNum);
        glGenVertexArrays(objNum, floorVAO.speedBumpVAOs.data());
        glGenBuffers(objNum, floorVAO.speedBumpVertexVBOs.data());
        glGenBuffers(objNum, floorVAO.speedBumpColorVBOs.data());
        glGenBuffers(objNum, floorVAO.speedBumpEBOs.data());
        HMIMap::bindSpeedBumpsData(hmi_map, floorName,
            floorVAO.speedBumpVAOs, floorVAO.speedBumpVertexVBOs, floorVAO.speedBumpColorVBOs, floorVAO.speedBumpEBOs);

        objNum = hmi_map.getRoads(floorName).size();
        floorVAO.roadVAOs.resize(objNum);
        floorVAO.roadVertexVBOs.resize(objNum);
        floorVAO.roadColorVBOs.resize(objNum);
        floorVAO.roadEBOs.resize(objNum);
        glGenVertexArrays(objNum, floorVAO.roadVAOs.data());
        glGenBuffers(objNum, floorVAO.roadVertexVBOs.data());
        glGenBuffers(objNum, floorVAO.roadColorVBOs.data());
        glGenBuffers(objNum, floorVAO.roadEBOs.data());
        floorVAO.roadPointNums = HMIMap::bindRoadsData(hmi_map, floorName,
            floorVAO.roadVAOs, floorVAO.roadVertexVBOs, floorVAO.roadColorVBOs, floorVAO.roadEBOs);

        floorVAOs.push_back(floorVAO);
    }

    ourShader.use();
    ourShader.setVec4("mainColor", 1.0f, 1.0f, 0.0f, 1.0f);

    float lastFrame = static_cast<float>(glfwGetTime());
    float deltaTime = 0.0f;
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        // input
        // -----
        processInput(window, deltaTime, mouseContext.camera);

        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // render the triangle
        ourShader.use();

        // pass projection matrix to shader (note that in this case it could change every frame)
        glm::mat4 projection = glm::perspective(glm::radians(mouseContext.camera.getZoom()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 500.0f);
        ourShader.setMat4("projection", projection);

        // camera/view transformation
        glm::mat4 view = mouseContext.camera.GetViewMatrix();
        ourShader.setMat4("view", view);

        glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        ourShader.setMat4("model", model);

        for (const auto &floorVAO : floorVAOs) {
            for (auto &vao : floorVAO.pillarVAOs) {
                glBindVertexArray(vao);
                glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
            }
            for (auto &vao : floorVAO.psdVAOs) {
                glBindVertexArray(vao);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            }

            for (auto &vao : floorVAO.speedBumpVAOs) {
                glBindVertexArray(vao);
                glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, 0);
            }

            for (std::size_t i = 0; i < floorVAO.roadVAOs.size(); ++i) {
                size_t vao = floorVAO.roadVAOs[i];
                size_t count = floorVAO.roadPointNums[i];
                glBindVertexArray(vao);
                glDrawElements(GL_LINE_STRIP, count, GL_UNSIGNED_INT, 0);
            }
        }
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    for (const auto &floorVAO : floorVAOs) {
        glDeleteVertexArrays(floorVAO.pillarVAOs.size(), floorVAO.pillarVAOs.data());
        glDeleteBuffers(floorVAO.pillarVertexVBOs.size(), floorVAO.pillarVertexVBOs.data());
        glDeleteBuffers(floorVAO.pillarColorVBOs.size(), floorVAO.pillarColorVBOs.data());
        glDeleteBuffers(floorVAO.pillarEBOs.size(), floorVAO.pillarEBOs.data());

        glDeleteVertexArrays(floorVAO.psdVAOs.size(), floorVAO.psdVAOs.data());
        glDeleteBuffers(floorVAO.psdVertexVBOs.size(), floorVAO.psdVertexVBOs.data());
        glDeleteBuffers(floorVAO.psdColorVBOs.size(), floorVAO.psdColorVBOs.data());
        glDeleteBuffers(floorVAO.psdEBOs.size(), floorVAO.psdEBOs.data());

        glDeleteVertexArrays(floorVAO.speedBumpVAOs.size(), floorVAO.speedBumpVAOs.data());
        glDeleteBuffers(floorVAO.speedBumpVertexVBOs.size(), floorVAO.speedBumpVertexVBOs.data());
        glDeleteBuffers(floorVAO.speedBumpColorVBOs.size(), floorVAO.speedBumpColorVBOs.data());
        glDeleteBuffers(floorVAO.speedBumpEBOs.size(), floorVAO.speedBumpEBOs.data());

        glDeleteVertexArrays(floorVAO.roadVAOs.size(), floorVAO.roadVAOs.data());
        glDeleteBuffers(floorVAO.roadVertexVBOs.size(), floorVAO.roadVertexVBOs.data());
        glDeleteBuffers(floorVAO.roadColorVBOs.size(), floorVAO.roadColorVBOs.data());
        glDeleteBuffers(floorVAO.roadEBOs.size(), floorVAO.roadEBOs.data());
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window, float deltaTime, Camera &camera)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS) {
        deltaTime = deltaTime * 0.1f;
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

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

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
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



