#include "navi_map/navi_map.h"

#include <string>
#include <iostream>

#include "utils/sql_util.h"
#include "utils/gl_util.h"


using namespace std;

struct TotalVAO {

    // 路径点
    std::vector<unsigned int> roadVAOs;
    std::vector<unsigned int> roadVertexVBOs;
    std::vector<unsigned int> roadColorVBOs;
    std::vector<unsigned int> roadPointNums;

    // 路口，出入口
    std::vector<unsigned int> poiVAOs;
    std::vector<unsigned int> poiVertexVBOs;
    std::vector<unsigned int> poiColorVBOs;
    std::vector<unsigned int> poiPointNums;
    // 减速带
    std::vector<unsigned int> roadMarkVAOs;
    std::vector<unsigned int> roadMarkVertexVBOs;
    std::vector<unsigned int> roadMarkColorVBOs;
    // 墙柱子
    std::vector<unsigned int> roadObstacleVAOs;
    std::vector<unsigned int> roadObstacleVertexVBOs;
    std::vector<unsigned int> roadObstacleColorVBOs;
    std::vector<unsigned int> roadObstaclePointNums;

    // 车位
    std::vector<unsigned int> psdVAOs;
    std::vector<unsigned int> psdVertexVBOs;
    std::vector<unsigned int> psdColorVBOs;
    std::vector<unsigned int> psdEBOs;
};

void glGen(int objNum, std::vector<unsigned int> &VAOs, std::vector<unsigned int> &VertexVBOs, std::vector<unsigned int> &ColorVBOs) {
    VAOs.resize(objNum);
    VertexVBOs.resize(objNum);
    ColorVBOs.resize(objNum);
    glGenVertexArrays(objNum, VAOs.data());
    glGenBuffers(objNum, VertexVBOs.data());
    glGenBuffers(objNum, ColorVBOs.data());
}

void glDelete(std::vector<unsigned int> &VAOs, std::vector<unsigned int> &VertexVBOs, std::vector<unsigned int> &ColorVBOs) {
    glDeleteVertexArrays(VAOs.size(), VAOs.data());
    glDeleteBuffers(VertexVBOs.size(), VertexVBOs.data());
    glDeleteBuffers(ColorVBOs.size(), ColorVBOs.data());
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        cout << "Usage: ./offline_navi_map <db_file> <partition_id>" << endl;
        return 1;
    }
    string db_path = argv[1];
    int partition_id = atoi(argv[2]);

    std::shared_ptr<navi_map::NaviMap> navi_map = navi_map::NaviMap::createNaviMap(db_path, partition_id,
                                                                                   navi_map::BlobType::LOC);
    auto startPoint = navi_map->getStartPoint();
    auto endPoint = navi_map->getEndPoint();
    GLUtil gl_util;
    gl_util.init(glm::vec3(startPoint[0], startPoint[1], startPoint[2] + 10),
                 glm::vec3(endPoint[0], endPoint[1], endPoint[2]));

    TotalVAO totalVAO;
    {
        size_t objNum{};
        objNum = navi_map->getRoads().size();
        glGen(objNum, totalVAO.roadVAOs, totalVAO.roadVertexVBOs, totalVAO.roadColorVBOs);
        totalVAO.roadPointNums = navi_map->bindRoadsData(
                totalVAO.roadVAOs, totalVAO.roadVertexVBOs, totalVAO.roadColorVBOs);

        objNum = navi_map->getPOI().size();
        glGen(objNum, totalVAO.poiVAOs, totalVAO.poiVertexVBOs, totalVAO.poiColorVBOs);
        totalVAO.poiPointNums = navi_map->bindPoiData(totalVAO.poiVAOs, totalVAO.poiVertexVBOs, totalVAO.poiColorVBOs);

        objNum = navi_map->getRoadMark().size();
        glGen(objNum, totalVAO.roadMarkVAOs, totalVAO.roadMarkVertexVBOs, totalVAO.roadMarkColorVBOs);
        navi_map->bindRoadMarkData(totalVAO.roadMarkVAOs, totalVAO.roadMarkVertexVBOs, totalVAO.roadMarkColorVBOs);

        objNum = navi_map->getRoadObstacle().size();
        glGen(objNum, totalVAO.roadObstacleVAOs, totalVAO.roadObstacleVertexVBOs, totalVAO.roadObstacleColorVBOs);
        totalVAO.roadObstaclePointNums = navi_map->bindRoadObstacleData(totalVAO.roadObstacleVAOs, totalVAO.roadObstacleVertexVBOs, totalVAO.roadObstacleColorVBOs);

        objNum = navi_map->getParkingSpaces().size();
        totalVAO.psdVAOs.resize(objNum);
        totalVAO.psdVertexVBOs.resize(objNum);
        totalVAO.psdColorVBOs.resize(objNum);
        totalVAO.psdEBOs.resize(objNum);
        glGenVertexArrays(objNum, totalVAO.psdVAOs.data());
        glGenBuffers(objNum, totalVAO.psdVertexVBOs.data());
        glGenBuffers(objNum, totalVAO.psdColorVBOs.data());
        glGenBuffers(objNum, totalVAO.psdEBOs.data());
        navi_map->bindPsdsData(
                totalVAO.psdVAOs, totalVAO.psdVertexVBOs, totalVAO.psdColorVBOs, totalVAO.psdEBOs);
    }


    float lastFrame = static_cast<float>(glfwGetTime());
    float deltaTime = 0.0f;

    while (!glfwWindowShouldClose(gl_util.window())) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        gl_util.processInput(deltaTime);

        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        gl_util.updateTransforms();

        {
            glPointSize(10.0f);

            for (size_t i = 0; i < totalVAO.roadVAOs.size(); ++i) {
                size_t vao = totalVAO.roadVAOs[i];
                size_t count = totalVAO.roadPointNums[i];
                glBindVertexArray(vao);
                glDrawArrays(GL_LINES, 0, count);
            }

            for (size_t i = 0; i < totalVAO.poiVAOs.size(); ++i) {
                size_t vao = totalVAO.poiVAOs[i];
                size_t count = totalVAO.poiPointNums[i];
                glBindVertexArray(vao);
                if (count == 1) {
                    glDrawArrays(GL_POINTS, 0, count);
                } else if (count == 2) {
                    glDrawArrays(GL_LINES, 0, count);
                } else {
                    glDrawArrays(GL_TRIANGLE_FAN, 0, count);
                }
            }

            for (size_t i = 0; i < totalVAO.roadMarkVAOs.size(); ++i) {
                size_t vao = totalVAO.roadMarkVAOs[i];
                glBindVertexArray(vao);
                glDrawArrays(GL_LINES, 0, 2);
            }

            for (size_t i = 0; i < totalVAO.roadObstacleVAOs.size(); ++i) {
                size_t vao = totalVAO.roadObstacleVAOs[i];
                size_t count = totalVAO.roadObstaclePointNums[i];
                glBindVertexArray(vao);
                if (count == 2) {
                    glDrawArrays(GL_LINES, 0, count);
                } else {
                    glDrawArrays(GL_TRIANGLE_FAN, 0, count);
                }
            }

            for (auto &vao: totalVAO.psdVAOs) {
                glBindVertexArray(vao);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            }
        }

        glfwSwapBuffers(gl_util.window());
        glfwPollEvents();
    }

    {
        glDelete(totalVAO.roadVAOs, totalVAO.roadVertexVBOs, totalVAO.roadColorVBOs);
        glDelete(totalVAO.poiVAOs, totalVAO.poiVertexVBOs, totalVAO.poiColorVBOs);
        glDelete(totalVAO.roadMarkVAOs, totalVAO.roadMarkVertexVBOs, totalVAO.roadMarkColorVBOs);
        glDelete(totalVAO.roadObstacleVAOs, totalVAO.roadObstacleVertexVBOs, totalVAO.roadObstacleColorVBOs);

        glDeleteVertexArrays(totalVAO.psdVAOs.size(), totalVAO.psdVAOs.data());
        glDeleteBuffers(totalVAO.psdVertexVBOs.size(), totalVAO.psdVertexVBOs.data());
        glDeleteBuffers(totalVAO.psdColorVBOs.size(), totalVAO.psdColorVBOs.data());
        glDeleteBuffers(totalVAO.psdEBOs.size(), totalVAO.psdEBOs.data());
    }

    glfwTerminate();
    return 0;
}
