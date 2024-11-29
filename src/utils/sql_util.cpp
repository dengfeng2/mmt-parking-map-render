#include "sql_util.h"
#include <iostream>

#include <sqlite3.h>

std::string query_for_render_data(const std::string &db_path, int partition_id) {
    std::string render_data{};
    sqlite3 *db;
    std::cout << "Opening database " << db_path << std::endl;
    if (sqlite3_open(db_path.c_str(), &db)) {
        std::cerr << "Can't open database: " << std::endl;
        return render_data;
    }

    std::string sql = "SELECT render_data FROM LPNP_table WHERE partition_id = "
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
