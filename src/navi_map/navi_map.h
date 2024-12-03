#ifndef NAVI_MAP_H
#define NAVI_MAP_H

#include <memory>
#include <vector>
#include <array>

namespace navi_map {

    struct Road {
        uint32_t id;
        float length;
        std::vector<float> road_center;  // [lon, lat, alt, lon, lat, alt, ...]
    };

    struct POI {
        enum POIType {
            UNKNOWN = 0,
            BUS_STOP = 1,
            BARRIER_GAP = 2,
            // FLOORS = 3; => guidance_data::ParkingFacility
            HUMAN_ACCESS = 4,
            GARAGE_ENTRANCE = 5,
            GARAGE_EXIT = 6,
            FACILITY_ENTRANCE = 7,
            FACILITY_EXIT = 8,
            FACILITY_EXIT_AND_ENTRANCE = 9,
            GARAGE_ENTRANCE_EXIT = 10,
            MARKET_ENTRANCE = 11,
            ELEVATOR_ENTRANCE = 12,
            ESCALATOR_ENTRANCE = 13,
            STOPPING_AREA = 14,
            // SUB_REGION = 15; => guidance_data::ParkingFacility
            EMERGENCY_PARKING_AREA = 16,
            EMERGENCY_ESCAPE_AREA = 17,
            TOLL_GATE = 18,
            CHECK_POINT = 19,
            SERVICE_AREA = 20,
            CONSTRUCTION_AREA = 21,
            PARK_INTERSECTION = 22, // 地库内路口
            HILL = 23,  // 地库内坡道
        };
        uint32_t id;
        POIType poi_type;
        std::vector<float> points;
    };

    struct RoadMark {
        enum RoadMarkType {
            UNKNOWN = 0,
            SPEED_BUMP = 1,
            STOP_LINE = 2,
            GUIDE_ARROW = 3,
            CROSSWALK = 4,
            // DASHED_SEGMENT = 5; => localization_data::DashedLineEndpoints
            CENTRAL_CIRCLE = 6,
            NO_PARKING_ZONE = 7,
            INDICATED_LINE = 8,
            LATERAL_DECELERATION_MARKING = 9,
            SYMBOL = 10,
            TEXT = 11,
        };
        uint32_t id;
        RoadMarkType type;
        std::vector<float> points;
    };

    struct RoadObstacle {
        enum RoadObstacleType {
            UNKNOWN = 0,
            ISOLATION_ISLAND = 1,
            SAFETY_ISLAND = 2,
            PORTABLE_TRAFFIC_LIGHT = 3,
            POLE_IN_ROAD = 4,
            TRAFFIC_CONTROL_BOX = 5,
            PILLAR = 6,
            // for LPNP
            WALL = 7,
            FENCE = 8,
            STEP = 9,
            SPECIAL = 10,
        };
        uint32_t id;
        RoadObstacleType type;
        std::vector<float> points;
    };

    struct ParkingSpace {
        uint32_t id;
        std::vector<float> points;
    };

    enum class BlobType {
        NAVI, LOC
    };

    class NaviMap {
    public:
        virtual ~NaviMap() = default;

        static std::shared_ptr<NaviMap> createNaviMap(const std::string &db_path, int partition_id, BlobType blob_type);

        [[nodiscard]] virtual std::array<double, 3> getStartPoint() const = 0;

        [[nodiscard]] virtual std::array<double, 3> getEndPoint() const = 0;

        [[nodiscard]] virtual std::vector<Road> getRoads() const = 0;

        virtual std::vector<unsigned int> bindRoadsData(
                const std::vector<unsigned int> &VAOs,
                const std::vector<unsigned int> &vertexVBOs,
                const std::vector<unsigned int> &colorVBOs) = 0;

        [[nodiscard]] virtual std::vector<POI> getPOI() const = 0;

        virtual std::vector<unsigned int> bindPoiData(const std::vector<unsigned int> &VAOs,
                                 const std::vector<unsigned int> &vertexVBOs,
                                 const std::vector<unsigned int> &colorVBOs) = 0;

        [[nodiscard]] virtual std::vector<RoadMark> getRoadMark() const = 0;

        virtual void bindRoadMarkData(const std::vector<unsigned int> &VAOs,
                                      const std::vector<unsigned int> &vertexVBOs,
                                      const std::vector<unsigned int> &colorVBOs) = 0;

        [[nodiscard]] virtual std::vector<RoadObstacle> getRoadObstacle() const = 0;

        virtual std::vector<unsigned int> bindRoadObstacleData(const std::vector<unsigned int> &VAOs,
                                          const std::vector<unsigned int> &vertexVBOs,
                                          const std::vector<unsigned int> &colorVBOs) = 0;

        [[nodiscard]] virtual std::vector<ParkingSpace> getParkingSpaces() const = 0;

        virtual void bindPsdsData(
                const std::vector<unsigned int> &VAOs,
                const std::vector<unsigned int> &vertexVBOs,
                const std::vector<unsigned int> &colorVBOs,
                const std::vector<unsigned int> &indicesEBOs) = 0;
    };
};


#endif //NAVI_MAP_H
