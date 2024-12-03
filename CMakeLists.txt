cmake_minimum_required(VERSION 3.29)
project(mmt_parking_map_render)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_subdirectory(libs/glfw)
add_subdirectory(libs/glad)
add_subdirectory(libs/glm)
add_subdirectory(libs/json)

set(protobuf_BUILD_TESTS OFF CACHE BOOL "" FORCE)  # 不构建测试
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)     # 构建静态库而非动态库
set(protobuf_BUILD_SHARED_LIBS OFF CACHE BOOL "Build protobuf as a static library")
set(ABSL_PROPAGATE_CXX_STD ON)
add_subdirectory(libs/protobuf)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
add_subdirectory(src)




