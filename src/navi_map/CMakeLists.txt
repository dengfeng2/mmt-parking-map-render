cmake_minimum_required(VERSION 3.29)

add_library(navi_map STATIC
        navi_map_impl.cpp)
target_link_libraries(navi_map PUBLIC
        nlohmann_json::nlohmann_json
        glfw
        glad
        glm
        util
        trans_util
)