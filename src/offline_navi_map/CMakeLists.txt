cmake_minimum_required(VERSION 3.29)

add_executable(offline_navi_map offline_navi_map.cpp)
target_link_libraries(offline_navi_map PUBLIC
        glfw
        glad
        glm
        util
        navi_map
        protobuf::libprotobuf
)