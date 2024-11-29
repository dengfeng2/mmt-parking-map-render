cmake_minimum_required(VERSION 3.29)

find_package(SQLite3 REQUIRED)

add_library(util STATIC sql_util.cpp)
include_directories(${SQLite3_INCLUDE_DIRS})
target_link_libraries(util ${SQLite3_LIBRARIES})