#ifndef HMI_MAP_IMPL_H
#define HMI_MAP_IMPL_H

#include "hmi_map.h"
#include <nlohmann/json.hpp>

#include <unordered_map>

class HmiMapImpl : public HMIMap {
public:
    HmiMapImpl(const std::string& data, LoadType loadType);

    [[nodiscard]] std::vector<float> getFloorNames() const override;

    [[nodiscard]] std::vector<Pillar> getPillars(float floorName) const override;

    [[nodiscard]] std::vector<Psd> getPsds(float floorName) const override;

    [[nodiscard]] std::vector<SpeedBump> getSpeedBumps(float floorName) const override;

    [[nodiscard]] std::vector<Road> getRoads(float floorName) const override;

    [[nodiscard]] int getTargetId() const override { return targetPrkId;};

    [[nodiscard]] std::array<float, 3> getStartPoint() const override { return startPoint; };

    [[nodiscard]] std::array<float, 3> getEndPoint() const override { return endPoint; };


    void bindPillarsData(float floorName,
                         const std::vector<unsigned int> &VAOs,
                         const std::vector<unsigned int> &vertexVBOs,
                         const std::vector<unsigned int> &colorVBOs,
                         const std::vector<unsigned int> &indicesEBOs) override;
    void bindPsdsData(float floorName,
        const std::vector<unsigned int> &VAOs,
        const std::vector<unsigned int> &vertexVBOs,
        const std::vector<unsigned int> &colorVBOs,
        const std::vector<unsigned int> &indicesEBOs) override;
    void bindSpeedBumpsData(float floorName,
        const std::vector<unsigned int> &VAOs,
        const std::vector<unsigned int> &vertexVBOs,
        const std::vector<unsigned int> &colorVBOs,
        const std::vector<unsigned int> &indicesEBOs) override;
    std::vector<unsigned int> bindRoadsData(float floorName,
        const std::vector<unsigned int> &VAOs,
        const std::vector<unsigned int> &vertexVBOs,
        const std::vector<unsigned int> &colorVBOs,
        const std::vector<unsigned int> &indicesEBOs) override;

private:
    void init(const nlohmann::json& data);
    std::unordered_map<float, Floor> floorData;
    int targetPrkId;
    std::array<float, 3> startPoint;
    std::array<float, 3> endPoint;
};


#endif //HMI_MAP_IMPL_H
