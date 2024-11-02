#include "hmi_map.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <numeric>

using namespace nlohmann;

HMIMap::HMIMap(const std::string &fileName) {
    std::ifstream f(fileName);
    json data = json::parse(f);
    json floors = data["floor"];

    for (auto & it : floors) {
        Floor floor;
        auto floorName = it.at("floorName");
        auto pillars = it.at("pillar");
        auto psds = it.at("psd");
        auto speedBumps = it.at("speedBump");
        auto roads = it.at("road");

        floor.floorName = floorName;
        for (const auto &pillar : pillars) {
            Pillar p;
            p.pillarId = pillar.at("pillarId");
            for (const auto &id : pillar.at("roadId")) {
                p.road.push_back(id);
            }
            for (const auto &point : pillar.at("points")) {
                p.points.push_back(point.at("x"));
                p.points.push_back(point.at("y"));
                p.points.push_back(point.at("z"));
            }
            floor.pillars.push_back(p);
        }
        for (const auto &psd : psds) {
            Psd p;
            p.psdId = psd.at("psdId");
            for (const auto &id : psd.at("roadId")) {
                p.road.push_back(id);
            }
            for (const auto &point : psd.at("points")) {
                p.points.push_back(point.at("x"));
                p.points.push_back(point.at("y"));
                p.points.push_back(point.at("z"));
            }
            floor.psds.push_back(p);
        }
        for (const auto &speedBump : speedBumps) {
            SpeedBump p;
            p.speedBumpId = speedBump.at("speedBumpId");
            for (const auto &id : speedBump.at("roadId")) {
                p.road.push_back(id);
            }
            for (const auto &point : speedBump.at("points")) {
                p.points.push_back(point.at("x"));
                p.points.push_back(point.at("y"));
                p.points.push_back(point.at("z"));
            }
            floor.speedBumps.push_back(p);
        }
        for (const auto &road : roads) {
            Road p;
            p.roadId = road.at("roadId");
            p.slopeType = road.at("slopeType");
            for (const auto &center : road.at("roadCenter")) {
                p.roadCenter.push_back(center.at("point").at("x"));
                p.roadCenter.push_back(center.at("point").at("y"));
                p.roadCenter.push_back(center.at("point").at("z"));
            }
            floor.roads.push_back(p);
        }

        floorData.emplace(floorName, floor);
    }

    json info = data["info"];
    startPoint[0] = info["learningStart"].at("x");
    startPoint[1] = info["learningStart"].at("y");
    startPoint[2] = info["learningStart"].at("z");
    endPoint[0] = info["learningEnd"].at("x");
    endPoint[1] = info["learningEnd"].at("y");
    endPoint[2] = info["learningEnd"].at("z");

    targetPrkId = info["targetPrk"].at("targetPrkId");

    std::cout << "Loaded hmi map" << std::endl;
}

std::vector<float> HMIMap::getFloorNames() const {
    std::vector<float> floorNames;
    floorNames.reserve(floorData.size());
    for (const auto &it : floorData) {
        floorNames.push_back(it.first);
    }
    return floorNames;
}

std::vector<Pillar> HMIMap::getPillars(float floorName) const {
    return floorData.at(floorName).pillars;
}

std::vector<Psd> HMIMap::getPsds(float floorName) const {
    return floorData.at(floorName).psds;
}

std::vector<SpeedBump> HMIMap::getSpeedBumps(float floorName) const {
    return floorData.at(floorName).speedBumps;
}

std::vector<Road> HMIMap::getRoads(float floorName) const {
    return floorData.at(floorName).roads;
}

void HMIMap::bindPillarsData(const HMIMap &hmi_map, float floorName,
    const std::vector<unsigned int> &VAOs,
    const std::vector<unsigned int> &vertexVBOs,
    const std::vector<unsigned int> &colorVBOs,
    const std::vector<unsigned int> &indicesEBOs) {

    std::vector<float> colors = {
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.9f, 0.5f, 0.2f,
        1.0f, 0.9f, 0.5f, 0.2f,
        1.0f, 0.9f, 0.5f, 0.2f,
        1.0f, 0.9f, 0.5f, 0.2f,
    };
    std::vector<unsigned int> indices = {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4,
        7, 3, 0, 0, 4, 7,
        6, 2, 1, 1, 5, 6,
        0, 1, 5, 5, 4, 0,
        3, 2, 6, 6, 7, 3,
    };

    auto pillars = hmi_map.getPillars(floorName);
    for (size_t i = 0; i < pillars.size(); i++) {
        std::vector<float> pillarsDataExpand;
        std::copy(pillars[i].points.begin(), pillars[i].points.end(), std::back_inserter(pillarsDataExpand));
        std::copy(pillars[i].points.begin(), pillars[i].points.end(), std::back_inserter(pillarsDataExpand));
        pillarsDataExpand[14] += 3.0;
        pillarsDataExpand[17] += 3.0;
        pillarsDataExpand[20] += 3.0;
        pillarsDataExpand[23] += 3.0;

        glBindVertexArray(VAOs[i]);

        glBindBuffer(GL_ARRAY_BUFFER, vertexVBOs[i]);
        glBufferData(GL_ARRAY_BUFFER, pillarsDataExpand.size() * sizeof(float), pillarsDataExpand.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, colorVBOs[i]);
        glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(float), colors.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesEBOs[i]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        glBindVertexArray(0);
    }
}

void HMIMap::bindPsdsData(const HMIMap &hmi_map, float floorName,
    const std::vector<unsigned int> &VAOs,
    const std::vector<unsigned int> &vertexVBOs,
    const std::vector<unsigned int> &colorVBOs,
    const std::vector<unsigned int> &indicesEBOs) {

    std::vector<float> colors = {
        0.8f, 0.8f, 0.8f, 1.0f,
        0.3f, 0.3f, 0.3f, 1.0f,
        0.3f, 0.3f, 0.3f, 1.0f,
        0.8f, 0.8f, 0.8f, 1.0f,
    };
    std::vector<float> targetColors = {
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.55f, 0.0f, 1.0f,
        1.0f, 0.55f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
    };
    std::vector<unsigned int> indices = {
        0, 1, 2, 2, 3, 0,
    };

    auto psds = hmi_map.getPsds(floorName);
    for (size_t i = 0; i < psds.size(); i++) {

        glBindVertexArray(VAOs[i]);

        glBindBuffer(GL_ARRAY_BUFFER, vertexVBOs[i]);
        glBufferData(GL_ARRAY_BUFFER, psds[i].points.size() * sizeof(float), psds[i].points.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, colorVBOs[i]);
        if (psds[i].psdId == hmi_map.getTargetId()) {
            glBufferData(GL_ARRAY_BUFFER, targetColors.size() * sizeof(float), targetColors.data(), GL_STATIC_DRAW);
        } else {
            glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(float), colors.data(), GL_STATIC_DRAW);
        }
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesEBOs[i]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        glBindVertexArray(0);
    }
}

void HMIMap::bindSpeedBumpsData(const HMIMap &hmi_map, float floorName,
    const std::vector<unsigned int> &VAOs,
    const std::vector<unsigned int> &vertexVBOs,
    const std::vector<unsigned int> &colorVBOs,
    const std::vector<unsigned int> &indicesEBOs) {

    std::vector<float> colors = {
        1.0f, 0.83f, 0.01f, 1.0f,
        1.0f, 0.83f, 0.01f, 1.0f,
    };
    std::vector<unsigned int> indices = {
        0, 1,
    };

    auto speedBumps = hmi_map.getSpeedBumps(floorName);
    for (size_t i = 0; i < speedBumps.size(); i++) {

        glBindVertexArray(VAOs[i]);

        glBindBuffer(GL_ARRAY_BUFFER, vertexVBOs[i]);
        glBufferData(GL_ARRAY_BUFFER, speedBumps[i].points.size() * sizeof(float), speedBumps[i].points.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, colorVBOs[i]);
        glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(float), colors.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesEBOs[i]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        glBindVertexArray(0);
    }
}

std::vector<unsigned int> HMIMap::bindRoadsData(const HMIMap &hmi_map, float floorName,
    const std::vector<unsigned int> &VAOs,
    const std::vector<unsigned int> &vertexVBOs,
    const std::vector<unsigned int> &colorVBOs,
    const std::vector<unsigned int> &indicesEBOs) {

    std::vector<float> colors = {
        0.0f, 0.7f, 1.0f, 1.0f,
    };
    std::vector<float> slopeColors = {
        0.0f, 0.0f, 1.0f, 1.0f,
    };

    std::vector<unsigned int> roadPointNums{};

    auto roads = hmi_map.getRoads(floorName);
    for (size_t i = 0; i < roads.size(); i++) {
        auto num = roads[i].roadCenter.size() / 3;
        roadPointNums.push_back(num);

        std::vector<float> colorExpand;
        for (size_t j = 0; j < num; j++) {
            if (roads[i].slopeType) {
                std::copy(slopeColors.begin(), slopeColors.end(), std::back_inserter(colorExpand));
            } else {
                std::copy(colors.begin(), colors.end(), std::back_inserter(colorExpand));
            }
        }

        std::vector<unsigned int> indices(num);
        std::iota(indices.begin(), indices.end(), 0);

        glBindVertexArray(VAOs[i]);

        glBindBuffer(GL_ARRAY_BUFFER, vertexVBOs[i]);
        glBufferData(GL_ARRAY_BUFFER, roads[i].roadCenter.size() * sizeof(float), roads[i].roadCenter.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, colorVBOs[i]);
        glBufferData(GL_ARRAY_BUFFER, colorExpand.size() * sizeof(float), colorExpand.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesEBOs[i]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        glBindVertexArray(0);
    }
    return roadPointNums;
}



