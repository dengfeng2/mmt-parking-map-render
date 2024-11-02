#ifndef HMI_MAP_H
#define HMI_MAP_H

#include<unordered_map>
#include<vector>

struct Pillar {
    int pillarId{};
    std::vector<int> road;
    std::vector<float> points;
};

struct Psd {
    int psdId{};
    std::vector<int> road;
    std::vector<float> points;
};

struct SpeedBump {
    int speedBumpId{};
    std::vector<int> road;
    std::vector<float> points;
};

struct Road {
    int roadId{};
    int slopeType{};
    std::vector<float> roadCenter;
};

struct Floor {
    float floorName{};
    std::vector<Pillar> pillars;
    std::vector<Psd> psds;
    std::vector<SpeedBump> speedBumps;
    std::vector<Road> roads;
};

class HMIMap {
  public:
    HMIMap(const std::string& fileName);

    std::vector<float> getFloorNames() const;

    std::vector<Pillar> getPillars(float floorName) const;

    std::vector<Psd> getPsds(float floorName) const;

    std::vector<SpeedBump> getSpeedBumps(float floorName) const;

    std::vector<Road> getRoads(float floorName) const;

    int getTargetId() const {
        return targetPrkId;
    }

    std::array<float, 3> getStartPoint() const {
        return startPoint;
    }
    std::array<float, 3> getEndPoint() const {
        return endPoint;
    }

    static void bindPillarsData(const HMIMap &hmi_map, float floorName,
        const std::vector<unsigned int> &VAOs,
        const std::vector<unsigned int> &vertexVBOs,
        const std::vector<unsigned int> &colorVBOs,
        const std::vector<unsigned int> &indicesEBOs);
    static void bindPsdsData(const HMIMap &hmi_map, float floorName,
        const std::vector<unsigned int> &VAOs,
        const std::vector<unsigned int> &vertexVBOs,
        const std::vector<unsigned int> &colorVBOs,
        const std::vector<unsigned int> &indicesEBOs);
    static void bindSpeedBumpsData(const HMIMap &hmi_map, float floorName,
        const std::vector<unsigned int> &VAOs,
        const std::vector<unsigned int> &vertexVBOs,
        const std::vector<unsigned int> &colorVBOs,
        const std::vector<unsigned int> &indicesEBOs);
    static std::vector<unsigned int> bindRoadsData(const HMIMap &hmi_map, float floorName,
        const std::vector<unsigned int> &VAOs,
        const std::vector<unsigned int> &vertexVBOs,
        const std::vector<unsigned int> &colorVBOs,
        const std::vector<unsigned int> &indicesEBOs);
  private:
    std::unordered_map<float, Floor> floorData;
    int targetPrkId;
    std::array<float, 3> startPoint;
    std::array<float, 3> endPoint;
};

#endif //HMI_MAP_H
