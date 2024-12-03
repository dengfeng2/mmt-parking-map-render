cmake_minimum_required(VERSION 3.29)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(road_tile)
add_subdirectory(utils)
add_subdirectory(hmi_map)
add_subdirectory(offline_hmi_map)
add_subdirectory(navi_map)
add_subdirectory(offline_navi_map)