cmake_minimum_required(VERSION 3.29)

add_executable(offline_hmi_map offline_hmi_map.cpp)
target_link_libraries(offline_hmi_map glfw glad glm hmi_map)
