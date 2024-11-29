cmake_minimum_required(VERSION 3.29)

add_library(hmi_map STATIC hmi_map_impl.cpp)
target_link_libraries(hmi_map nlohmann_json::nlohmann_json glfw glad glm)