#ifndef SQL_UTIL_H
#define SQL_UTIL_H

#include <string>
#include <array>
#include "road_tile.pb.h"

std::string query_for_column(const std::string &db_path, int partition_id, const std::string &col);
hdmap::data::proto::RoadTile query_for_road_tile(const std::string &db_path, int partition_id, const std::string &col);
std::array<double, 3> getPoint(const std::string &str);
std::array<int, 3> getTargetPrkId(const std::string &str);
#endif //SQL_UTIL_H
