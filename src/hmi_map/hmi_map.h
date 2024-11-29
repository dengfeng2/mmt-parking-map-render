#ifndef HMI_MAP_H
#define HMI_MAP_H

#include<vector>
#include<array>
#include<memory>

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

enum class LoadType {
    FILE,
    STRING,
};

class HMIMap {
  public:
    static std::shared_ptr<HMIMap> createHmiMap(const std::string& data, LoadType loadType);
    virtual ~HMIMap() = default;

    [[nodiscard]] virtual std::vector<float> getFloorNames() const = 0;
    [[nodiscard]] virtual std::vector<Pillar> getPillars(float floorName) const = 0;
    [[nodiscard]] virtual std::vector<Psd> getPsds(float floorName) const = 0;
    [[nodiscard]] virtual std::vector<SpeedBump> getSpeedBumps(float floorName) const = 0;
    [[nodiscard]] virtual std::vector<Road> getRoads(float floorName) const = 0;
    [[nodiscard]] virtual int getTargetId() const = 0;
    [[nodiscard]] virtual std::array<float, 3> getStartPoint() const = 0;
    [[nodiscard]] virtual std::array<float, 3> getEndPoint() const = 0;

    virtual void bindPillarsData(float floorName,
        const std::vector<unsigned int> &VAOs,
        const std::vector<unsigned int> &vertexVBOs,
        const std::vector<unsigned int> &colorVBOs,
        const std::vector<unsigned int> &indicesEBOs) = 0;
    virtual void bindPsdsData(float floorName,
        const std::vector<unsigned int> &VAOs,
        const std::vector<unsigned int> &vertexVBOs,
        const std::vector<unsigned int> &colorVBOs,
        const std::vector<unsigned int> &indicesEBOs) = 0;
    virtual void bindSpeedBumpsData(float floorName,
        const std::vector<unsigned int> &VAOs,
        const std::vector<unsigned int> &vertexVBOs,
        const std::vector<unsigned int> &colorVBOs,
        const std::vector<unsigned int> &indicesEBOs) = 0;
    virtual std::vector<unsigned int> bindRoadsData(float floorName,
        const std::vector<unsigned int> &VAOs,
        const std::vector<unsigned int> &vertexVBOs,
        const std::vector<unsigned int> &colorVBOs,
        const std::vector<unsigned int> &indicesEBOs) = 0;
};

#endif //HMI_MAP_H
