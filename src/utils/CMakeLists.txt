cmake_minimum_required(VERSION 3.29)

find_package(SQLite3 REQUIRED)


add_library(util STATIC
        sql_util.cpp
        gl_util.cpp
)
target_include_directories(util PUBLIC
        ${SQLite3_INCLUDE_DIRS}
)
target_link_libraries(util PUBLIC
        ${SQLite3_LIBRARIES}
        glfw glad glm
        road_tile
)

add_library(trans_util STATIC trans_util.cpp)
