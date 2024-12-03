#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <fstream>

#include "utils/gl_util.h"
#include "hmi_map/hmi_map.h"
#include "utils/sql_util.h"

using namespace std;

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
    /************** 处理命令输入，生成HMIMap对象 *************/
    if (argc != 2 && argc != 3) {
        cout << "Usage: \n./offline_hmi_map <map_file> \n./offline_hmi_map <db_file> <partition_id>" << endl;
        return 1;
    }
    std::string filename = argv[1];
    std::size_t dot_pos = filename.rfind('.');
    if (dot_pos == std::string::npos) {
        std::cerr << "Error: File with no extension." << std::endl;
        return 1;
    }
    shared_ptr<HMIMap> hmi_map;
    std::string extension = filename.substr(dot_pos); // 获取文件扩展名
    if (extension == ".json") {
        hmi_map = HMIMap::createHmiMap(filename, LoadType::FILE);
    } else if (extension == ".db") {
        if (argc != 3) {
            std::cerr << "Error: .db file provided but no <partition_id> was given." << std::endl;
            return 1;
        }
        int partition_id = atoi(argv[2]);
        std::string render_data = query_for_column(argv[1], partition_id, "render_data");
        hmi_map = HMIMap::createHmiMap(render_data, LoadType::STRING);
    } else {
        std::cerr << "Error: Unsupported file type. Only .json and .db are allowed." << std::endl;
        return 1;
    }
    /**************************************************/

    auto startPoint = hmi_map->getStartPoint();
    auto endPoint = hmi_map->getEndPoint();

    GLUtil gl_util;
    gl_util.init(glm::vec3(startPoint[0], startPoint[1], startPoint[2]+10),
        glm::vec3(endPoint[0], endPoint[1], endPoint[2]));


    std::vector<FloorVAO> floorVAOs;
    auto floorNames = hmi_map->getFloorNames();
    for (auto floorName : floorNames) {
        FloorVAO floorVAO;
        floorVAO.floorName = floorName;

        size_t objNum{};

        objNum = hmi_map->getPillars(floorName).size();
        floorVAO.pillarVAOs.resize(objNum);
        floorVAO.pillarVertexVBOs.resize(objNum);
        floorVAO.pillarColorVBOs.resize(objNum);
        floorVAO.pillarEBOs.resize(objNum);
        glGenVertexArrays(objNum, floorVAO.pillarVAOs.data());
        glGenBuffers(objNum, floorVAO.pillarVertexVBOs.data());
        glGenBuffers(objNum, floorVAO.pillarColorVBOs.data());
        glGenBuffers(objNum, floorVAO.pillarEBOs.data());
        hmi_map->bindPillarsData(floorName,
            floorVAO.pillarVAOs, floorVAO.pillarVertexVBOs, floorVAO.pillarColorVBOs, floorVAO.pillarEBOs);

        objNum = hmi_map->getPsds(floorName).size();
        floorVAO.psdVAOs.resize(objNum);
        floorVAO.psdVertexVBOs.resize(objNum);
        floorVAO.psdColorVBOs.resize(objNum);
        floorVAO.psdEBOs.resize(objNum);
        glGenVertexArrays(objNum, floorVAO.psdVAOs.data());
        glGenBuffers(objNum, floorVAO.psdVertexVBOs.data());
        glGenBuffers(objNum, floorVAO.psdColorVBOs.data());
        glGenBuffers(objNum, floorVAO.psdEBOs.data());
        hmi_map->bindPsdsData(floorName,
            floorVAO.psdVAOs, floorVAO.psdVertexVBOs, floorVAO.psdColorVBOs, floorVAO.psdEBOs);

        objNum = hmi_map->getSpeedBumps(floorName).size();
        floorVAO.speedBumpVAOs.resize(objNum);
        floorVAO.speedBumpVertexVBOs.resize(objNum);
        floorVAO.speedBumpColorVBOs.resize(objNum);
        floorVAO.speedBumpEBOs.resize(objNum);
        glGenVertexArrays(objNum, floorVAO.speedBumpVAOs.data());
        glGenBuffers(objNum, floorVAO.speedBumpVertexVBOs.data());
        glGenBuffers(objNum, floorVAO.speedBumpColorVBOs.data());
        glGenBuffers(objNum, floorVAO.speedBumpEBOs.data());
        hmi_map->bindSpeedBumpsData(floorName,
            floorVAO.speedBumpVAOs, floorVAO.speedBumpVertexVBOs, floorVAO.speedBumpColorVBOs, floorVAO.speedBumpEBOs);

        objNum = hmi_map->getRoads(floorName).size();
        floorVAO.roadVAOs.resize(objNum);
        floorVAO.roadVertexVBOs.resize(objNum);
        floorVAO.roadColorVBOs.resize(objNum);
        floorVAO.roadEBOs.resize(objNum);
        glGenVertexArrays(objNum, floorVAO.roadVAOs.data());
        glGenBuffers(objNum, floorVAO.roadVertexVBOs.data());
        glGenBuffers(objNum, floorVAO.roadColorVBOs.data());
        glGenBuffers(objNum, floorVAO.roadEBOs.data());
        floorVAO.roadPointNums = hmi_map->bindRoadsData(floorName,
            floorVAO.roadVAOs, floorVAO.roadVertexVBOs, floorVAO.roadColorVBOs, floorVAO.roadEBOs);

        floorVAOs.push_back(floorVAO);
    }



    float lastFrame = static_cast<float>(glfwGetTime());
    float deltaTime = 0.0f;
    // render loop
    // -----------
    while (!glfwWindowShouldClose(gl_util.window()))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        // input
        // -----
        gl_util.processInput(deltaTime);

        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        gl_util.updateTransforms();

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
        glfwSwapBuffers(gl_util.window());
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




