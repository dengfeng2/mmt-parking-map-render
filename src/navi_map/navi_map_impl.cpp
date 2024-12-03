#include "navi_map_impl.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "utils/sql_util.h"
#include <iostream>

namespace navi_map {
    std::shared_ptr<NaviMap> NaviMap::createNaviMap(const std::string &db_path, int partition_id, BlobType blob_type) {
        return std::make_shared<NaviMapImpl>(db_path, partition_id, blob_type);
    }

    NaviMapImpl::NaviMapImpl(const std::string &db_path, int partition_id, BlobType blob_type) {
        auto ref_point_str = query_for_column(db_path, partition_id, "ref_point");
        auto ref_point_wgs84 = getPoint(ref_point_str);
        auto ref_point_gcj02 = wgs84_to_gcj02(ref_point_wgs84[0], ref_point_wgs84[1]);
        auto ref_point = std::array<double, 3>{ref_point_gcj02[0], ref_point_gcj02[1], ref_point_wgs84[2]};
        trans_util_ = std::make_unique<TransUtil>(ref_point[0], ref_point[1], ref_point[2]);

        auto target_prk_id_str = query_for_column(db_path, partition_id, "target_prk_id");
        auto target_prk_id = getTargetPrkId(target_prk_id_str);
        target_prk_space_id_ = target_prk_id[2];
//        auto trace_start = query_for_column(db_path, partition_id, "trace_start");
        auto trace_dest = query_for_column(db_path, partition_id, "trace_dest");
//        auto start_point = getPoint(trace_start);
        auto end_point = getPoint(trace_dest);
        start_point_ = trans_util_->transToENU(ref_point[0], ref_point[1], ref_point[2]);
        end_point_ = trans_util_->transToENU(end_point[0], end_point[1], end_point[2]);
        std::cout << "ref:(" << start_point_[0] << "," << start_point_[1] << "," << start_point_[2] << ")" << std::endl;
        std::cout << "end:(" << end_point_[0] << "," << end_point_[1] << "," << end_point_[2] << ")" << std::endl;


        if (blob_type == BlobType::NAVI) {
            road_tile_ = query_for_road_tile(db_path, partition_id, "blob_data");
        } else {
            road_tile_ = query_for_road_tile(db_path, partition_id, "road_mark");
        }
    }

    std::array<double, 3> NaviMapImpl::getStartPoint() const {
        return start_point_;
    }

    std::array<double, 3> NaviMapImpl::getEndPoint() const {
        return end_point_;
    }

    std::vector<Road> NaviMapImpl::getRoads() const {
        std::vector<Road> roads;
        for (size_t i = 0; i < road_tile_.road_size(); ++i) {
            roads.push_back({});
            auto road = road_tile_.road(i);
            roads.back().id = road.id().count();
            roads.back().length = road.length();
            const auto &road_center = road.road_center();
            for (size_t j = 0; j < road_center.points_size(); j++) {
                auto p = trans_util_->transToENU(
                        road_center.points(j).longitude(),
                        road_center.points(j).latitude(),
                        road_center.points(j).altitude());
                roads.back().road_center.push_back(p[0]);
                roads.back().road_center.push_back(p[1]);
                roads.back().road_center.push_back(p[2]);
            }
        }
        return roads;
    }

    std::vector<unsigned int> NaviMapImpl::bindRoadsData(
            const std::vector<unsigned int> &VAOs,
            const std::vector<unsigned int> &vertexVBOs,
            const std::vector<unsigned int> &colorVBOs) {

        std::vector<float> color = {
                0.0f, 0.7f, 1.0f, 1.0f,
        };

        std::vector<unsigned int> roadPointNums{};
        std::vector<Road> roads = getRoads();
        for (size_t i = 0; i < roads.size(); ++i) {
            auto point_num = roads[i].road_center.size() / 3;
            roadPointNums.push_back(point_num);

            glBindVertexArray(VAOs[i]);

            glBindBuffer(GL_ARRAY_BUFFER, vertexVBOs[i]);
            glBufferData(GL_ARRAY_BUFFER, roads[i].road_center.size() * sizeof(float), roads[i].road_center.data(),
                         GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
            glEnableVertexAttribArray(0);

            std::vector<float> colors;
            for (size_t n = 0; n < point_num; n++) {
                colors.insert(colors.end(), color.begin(), color.end());
            }
            glBindBuffer(GL_ARRAY_BUFFER, colorVBOs[i]);
            glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(float), colors.data(), GL_STATIC_DRAW);
            glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) 0);
            glEnableVertexAttribArray(1);

            glBindVertexArray(0);  // reset
        }
        return roadPointNums;
    }


    std::vector<POI> NaviMapImpl::getPOI() const {
        std::vector<POI> pois;
        for (size_t i = 0; i < road_tile_.poi_size(); ++i) {
            pois.push_back({});
            auto poi = road_tile_.poi(i);
            pois.back().id = poi.id().count();
            pois.back().poi_type = static_cast<POI::POIType>(poi.poi_type());
            for (size_t j = 0; j < poi.shape_size(); j++) {
                auto p = trans_util_->transToENU(poi.shape(j).longitude(), poi.shape(j).latitude(),
                                                 poi.shape(j).altitude());
                pois.back().points.push_back(p[0]);
                pois.back().points.push_back(p[1]);
                pois.back().points.push_back(p[2]);
            }
        }
        return pois;
    }

    std::vector<unsigned int> NaviMapImpl::bindPoiData(const std::vector<unsigned int> &VAOs,
                                                       const std::vector<unsigned int> &vertexVBOs,
                                                       const std::vector<unsigned int> &colorVBOs) {

        std::vector<float> color = {1.0f, 1.0f, 1.0f, 1.0f,};
        std::vector<float> entrance_color = {1.0f, 0.27f, 0.0f, 1.0f,};
        std::vector<float> check_point_color = {0.0f, 0.5f, 0.0f, 1.0f,};
        std::vector<float> hill_color = {1.0f, 0.5f, 0.3f, 1.0f,};
        std::vector<float> intersection_color = {0.0f, 1.0f, 0.5f, 1.0f,};

        std::vector<unsigned int> poiPointNums{};
        auto poi = getPOI();
        for (size_t i = 0; i < poi.size(); i++) {
            glBindVertexArray(VAOs[i]);

            glBindBuffer(GL_ARRAY_BUFFER, vertexVBOs[i]);
            glBufferData(GL_ARRAY_BUFFER, poi[i].points.size() * sizeof(float), poi[i].points.data(), GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
            glEnableVertexAttribArray(0);

            auto point_num = poi[i].points.size() / 3;
            poiPointNums.push_back(point_num);
            std::vector<float> colors;
            if (poi[i].poi_type == POI::GARAGE_ENTRANCE) {
                for (size_t n = 0; n < point_num; n++) {
                    colors.insert(colors.end(), entrance_color.begin(), entrance_color.end());
                }
            } else if (poi[i].poi_type == POI::CHECK_POINT) {
                for (size_t n = 0; n < point_num; n++) {
                    colors.insert(colors.end(), check_point_color.begin(), check_point_color.end());
                }
            } else if (poi[i].poi_type == POI::HILL) {
                for (size_t n = 0; n < point_num; n++) {
                    colors.insert(colors.end(), hill_color.begin(), hill_color.end());
                }
            } else {
                for (size_t n = 0; n < point_num; n++) {
                    colors.insert(colors.end(), color.begin(), color.end());
                }
            }
            glBindBuffer(GL_ARRAY_BUFFER, colorVBOs[i]);
            glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(float), colors.data(), GL_STATIC_DRAW);
            glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) 0);
            glEnableVertexAttribArray(1);

            glBindVertexArray(0);  // reset
        }
        return poiPointNums;
    }

    std::vector<RoadMark> NaviMapImpl::getRoadMark() const {
        std::vector<RoadMark> road_marks;
        for (size_t i = 0; i < road_tile_.road_mark_size(); ++i) {
            road_marks.push_back({});
            auto road_mark = road_tile_.road_mark(i);
            road_marks.back().id = road_mark.id().count();
            road_marks.back().type = static_cast<RoadMark::RoadMarkType>(road_mark.type());
            for (size_t j = 0; j < road_mark.shape_size(); j++) {
                auto p = trans_util_->transToENU(
                        road_mark.shape(j).longitude(),
                        road_mark.shape(j).latitude(),
                        road_mark.shape(j).altitude());
                road_marks.back().points.push_back(p[0]);
                road_marks.back().points.push_back(p[1]);
                road_marks.back().points.push_back(p[2]);
            }
        }

        return road_marks;
    }

    void
    NaviMapImpl::bindRoadMarkData(const std::vector<unsigned int> &VAOs, const std::vector<unsigned int> &vertexVBOs,
                                  const std::vector<unsigned int> &colorVBOs) {
        std::vector<float> colors = {
                1.0f, 0.83f, 0.01f, 1.0f,
                1.0f, 0.83f, 0.01f, 1.0f,
        };
        auto roadMark = getRoadMark();
        for (size_t i = 0; i < roadMark.size(); i++) {
            glBindVertexArray(VAOs[i]);

            glBindBuffer(GL_ARRAY_BUFFER, vertexVBOs[i]);
            glBufferData(GL_ARRAY_BUFFER, roadMark[i].points.size() * sizeof(float), roadMark[i].points.data(),
                         GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
            glEnableVertexAttribArray(0);

            glBindBuffer(GL_ARRAY_BUFFER, colorVBOs[i]);
            glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(float), colors.data(), GL_STATIC_DRAW);
            glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) 0);
            glEnableVertexAttribArray(1);

            glBindVertexArray(0);
        }
    }

    std::vector<RoadObstacle> NaviMapImpl::getRoadObstacle() const {
        std::vector<RoadObstacle> road_obstacles;
        for (size_t i = 0; i < road_tile_.road_obstacle_size(); ++i) {
            road_obstacles.push_back({});
            auto obstacle = road_tile_.road_obstacle(i);
            road_obstacles.back().id = obstacle.id().count();
            road_obstacles.back().type = static_cast<RoadObstacle::RoadObstacleType>(obstacle.type());
            for (size_t j = 0; j < obstacle.shape_size(); j++) {
                auto p = trans_util_->transToENU(
                        obstacle.shape(j).longitude(),
                        obstacle.shape(j).latitude(),
                        obstacle.shape(j).altitude());
                road_obstacles.back().points.push_back(p[0]);
                road_obstacles.back().points.push_back(p[1]);
                road_obstacles.back().points.push_back(p[2]);
            }
        }

        return road_obstacles;
    }

    std::vector<unsigned int> NaviMapImpl::bindRoadObstacleData(const std::vector<unsigned int> &VAOs,
                                                                const std::vector<unsigned int> &vertexVBOs,
                                                                const std::vector<unsigned int> &colorVBOs) {

        std::vector<float> color = {1.0f, 1.0f, 1.0f, 1.0f,};
        std::vector<float> pillar_color = {1.0f, 0.56f, 0.0f, 1.0f,};
        std::vector<float> wall_color = {0.65f, 0.16f, 0.16f, 1.0f,};

        std::vector<unsigned int> roadObstaclePointNums{};
        auto road_obstacle = getRoadObstacle();
        for (size_t i = 0; i < road_obstacle.size(); i++) {
            glBindVertexArray(VAOs[i]);

            glBindBuffer(GL_ARRAY_BUFFER, vertexVBOs[i]);
            glBufferData(GL_ARRAY_BUFFER, road_obstacle[i].points.size() * sizeof(float),
                         road_obstacle[i].points.data(), GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
            glEnableVertexAttribArray(0);

            auto point_num = road_obstacle[i].points.size() / 3;
            roadObstaclePointNums.push_back(point_num);
            std::vector<float> colors;
            if (road_obstacle[i].type == RoadObstacle::RoadObstacleType::PILLAR) {
                for (size_t n = 0; n < point_num; n++) {
                    colors.insert(colors.end(), pillar_color.begin(), pillar_color.end());
                }
            } else if (road_obstacle[i].type == RoadObstacle::RoadObstacleType::WALL) {
                for (size_t n = 0; n < point_num; n++) {
                    colors.insert(colors.end(), wall_color.begin(), wall_color.end());
                }
            } else {
                for (size_t n = 0; n < point_num; n++) {
                    colors.insert(colors.end(), color.begin(), color.end());
                }
            }
            glBindBuffer(GL_ARRAY_BUFFER, colorVBOs[i]);
            glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(float), colors.data(), GL_STATIC_DRAW);
            glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) 0);
            glEnableVertexAttribArray(1);

            glBindVertexArray(0);  // reset
        }
        return roadObstaclePointNums;
    }

    std::vector<ParkingSpace> NaviMapImpl::getParkingSpaces() const {
        std::vector<ParkingSpace> parking_spaces;
        for (size_t i = 0; i < road_tile_.parking_space_size(); ++i) {
            parking_spaces.push_back({});
            auto pks = road_tile_.parking_space(i);
            parking_spaces.back().id = pks.id().count();

            for (size_t j = 0; j < pks.shape_size(); ++j) {
                auto p = trans_util_->transToENU(
                        pks.shape(j).longitude(),
                        pks.shape(j).latitude(),
                        pks.shape(j).altitude());
//                std::cout << p[0] << "," << p[1] << "," << p[2] << std::endl;
                parking_spaces.back().points.push_back(p[0]);
                parking_spaces.back().points.push_back(p[1]);
                parking_spaces.back().points.push_back(p[2]);
            }
        }

        return parking_spaces;
    }

    void NaviMapImpl::bindPsdsData(
            const std::vector<unsigned int> &VAOs,
            const std::vector<unsigned int> &vertexVBOs,
            const std::vector<unsigned int> &colorVBOs,
            const std::vector<unsigned int> &indicesEBOs) {

        std::vector<float> colors = {
                0.8f, 0.8f, 0.8f, 1.0f,
                0.8f, 0.8f, 0.8f, 1.0f,
                0.3f, 0.3f, 0.3f, 1.0f,
                0.3f, 0.3f, 0.3f, 1.0f,
        };
        std::vector<float> targetColors = {
                1.0f, 1.0f, 1.0f, 1.0f,
                1.0f, 1.0f, 1.0f, 1.0f,
                1.0f, 0.55f, 0.0f, 1.0f,
                1.0f, 0.55f, 0.0f, 1.0f,
        };
        std::vector<unsigned int> indices = {
                0, 1, 2, 2, 3, 0,
        };

        auto psds = getParkingSpaces();
        for (size_t i = 0; i < psds.size(); ++i) {
            glBindVertexArray(VAOs[i]);

            glBindBuffer(GL_ARRAY_BUFFER, vertexVBOs[i]);
            glBufferData(GL_ARRAY_BUFFER, psds[i].points.size() * sizeof(float), psds[i].points.data(), GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
            glEnableVertexAttribArray(0);

            glBindBuffer(GL_ARRAY_BUFFER, colorVBOs[i]);
            if (psds[i].id == target_prk_space_id_) {
                glBufferData(GL_ARRAY_BUFFER, targetColors.size() * sizeof(float), targetColors.data(), GL_STATIC_DRAW);
            } else {
                glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(float), colors.data(), GL_STATIC_DRAW);
            }
            glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) 0);
            glEnableVertexAttribArray(1);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesEBOs[i]);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(),
                         GL_STATIC_DRAW);

            glBindVertexArray(0);
        }
    }
} // namespace navi_map


