#ifndef SQL_UTIL_H
#define SQL_UTIL_H

#include <string>

std::string query_for_render_data(const std::string &db_path, int partition_id);
#endif //SQL_UTIL_H
