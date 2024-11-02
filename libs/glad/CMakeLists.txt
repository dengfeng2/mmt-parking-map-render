cmake_minimum_required(VERSION 3.29)
project(glad)

# 添加Glad库
add_library(glad STATIC "${CMAKE_CURRENT_SOURCE_DIR}/src/glad.c")

# 指定Glad头文件的位置
target_include_directories(glad PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")