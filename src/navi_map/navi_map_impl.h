#ifndef NAVI_MAP_IMPL_H
#define NAVI_MAP_IMPL_H

#include "navi_map.h"
#include "road_tile.pb.h"
#include <string>
#include <utils/trans_util.h>

namespace navi_map {
    class NaviMapImpl : public NaviMap {
    public:
        NaviMapImpl(const std::string &db_path, int partition_id, BlobType blob_type);

        [[nodiscard]] std::array<double, 3> getStartPoint() const override;

        [[nodiscard]] std::array<double, 3> getEndPoint() const override;

        [[nodiscard]] std::vector<Road> getRoads() const override;

        std::vector<unsigned int> bindRoadsData(
                const std::vector<unsigned int> &VAOs,
                const std::vector<unsigned int> &vertexVBOs,
                const std::vector<unsigned int> &colorVBOs) override;

        [[nodiscard]] std::vector<POI> getPOI() const override;

        std::vector<unsigned int> bindPoiData(const std::vector<unsigned int> &VAOs,
                         const std::vector<unsigned int> &vertexVBOs,
                         const std::vector<unsigned int> &colorVBOs) override;

        [[nodiscard]] std::vector<RoadMark> getRoadMark() const override;

        void bindRoadMarkData(const std::vector<unsigned int> &VAOs,
                              const std::vector<unsigned int> &vertexVBOs,
                              const std::vector<unsigned int> &colorVBOs) override;

        [[nodiscard]] std::vector<RoadObstacle> getRoadObstacle() const override;

        std::vector<unsigned int> bindRoadObstacleData(const std::vector<unsigned int> &VAOs,
                                  const std::vector<unsigned int> &vertexVBOs,
                                  const std::vector<unsigned int> &colorVBOs) override;

        [[nodiscard]] std::vector<ParkingSpace> getParkingSpaces() const override;

        void bindPsdsData(
                const std::vector<unsigned int> &VAOs,
                const std::vector<unsigned int> &vertexVBOs,
                const std::vector<unsigned int> &colorVBOs,
                const std::vector<unsigned int> &indicesEBOs) override;

    private:
        hdmap::data::proto::RoadTile road_tile_{};
        std::array<double, 3> start_point_{};
        std::array<double, 3> end_point_{};
        std::unique_ptr<TransUtil> trans_util_;
        int target_prk_space_id_;
    };
} // namespace navi_map

#endif //NAVI_MAP_IMPL_H
