#include "sql_util.h"
#include <iostream>

#include <sqlite3.h>

std::string query_for_column(const std::string &db_path, int partition_id, const std::string &col) {
    std::string render_data{};
    sqlite3 *db;
    if (sqlite3_open(db_path.c_str(), &db)) {
        std::cerr << "Can't open database: " << std::endl;
        return render_data;
    }

    std::string sql = "SELECT "+
        col +
            " FROM LPNP_table WHERE partition_id = "
                           + std::to_string(partition_id) + ";";

    char *err_msg = nullptr;
    auto ret = sqlite3_exec(db, sql.c_str(), [](void* data, int argc, char** argv, char** azColName)->int {
        auto* renderData = static_cast<std::string*>(data);
        if (argc > 0 && argv[0]) {
            *renderData = argv[0]; // 假设 SELECT 的第一个列就是 render_data
        }
        return 0;
    }, &render_data, &err_msg);

    if (ret != SQLITE_OK) {
        std::cerr << "SQL error: " << err_msg << std::endl;
        sqlite3_free(err_msg);
    }

    sqlite3_close(db);
    return render_data;
}

hdmap::data::proto::RoadTile query_for_road_tile(const std::string &db_path, int partition_id, const std::string &col) {
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int rc;

    // 打开数据库
    rc = sqlite3_open(db_path.c_str(), &db);
    if (rc != SQLITE_OK) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        throw std::runtime_error("Can't open database");
    }

    const char *sql_query = "SELECT blob_data FROM LPNP_table WHERE partition_id=?";
    rc = sqlite3_prepare_v2(db, sql_query, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        throw std::runtime_error("Failed to execute statement");
    }
    sqlite3_bind_int(stmt, 1, partition_id);

    hdmap::data::proto::RoadTile road_tile;
    if ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int bytes = sqlite3_column_bytes(stmt, 0);
        const void *blob = sqlite3_column_blob(stmt, 0);

        // 直接使用从数据库中获取的BLOB数据来反序列化proto对象
        if (!road_tile.ParseFromArray(blob, bytes)) {
            throw std::runtime_error("Failed to parse the BLOB data into a proto object.");
        }
    } else if (rc == SQLITE_DONE) {
        throw std::runtime_error("No data found for partition_id");
    } else {
        throw std::runtime_error("Failed to retrieve data");
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return road_tile;
}

std::array<double, 3> getPoint(const std::string &str) {
    std::stringstream ss(str);
    std::string token;
    std::vector<double> point;
    while (std::getline(ss, token, ',')) {
        try {
            double val = std::stod(token);
            point.push_back(val);
        } catch (const std::invalid_argument& ia) {
            std::cerr << "Invalid argument: " << ia.what() << '\n';
        } catch (const std::out_of_range& oor) {
            std::cerr << "Out of Range error: " << oor.what() << '\n';
        }
    }
    return std::array<double, 3>{point[0], point[1], point[2]};
}

std::array<int, 3> getTargetPrkId(const std::string &str) {
    std::stringstream ss(str);
    std::string token;
    std::vector<int> point;
    while (std::getline(ss, token, ',')) {
        try {
            int val = std::stoi(token);
            point.push_back(val);
        } catch (const std::invalid_argument& ia) {
            std::cerr << "Invalid argument: " << ia.what() << '\n';
        } catch (const std::out_of_range& oor) {
            std::cerr << "Out of Range error: " << oor.what() << '\n';
        }
    }
    return std::array<int, 3>{point[0], point[1], point[2]};
}
