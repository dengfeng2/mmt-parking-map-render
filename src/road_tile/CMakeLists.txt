cmake_minimum_required(VERSION 3.29)

set(GENERATED_PROTOBUF_PATH "${CMAKE_CURRENT_BINARY_DIR}")
add_custom_command(
        OUTPUT "${GENERATED_PROTOBUF_PATH}/road_tile.pb.cc"
               "${GENERATED_PROTOBUF_PATH}/road_tile.pb.h"
        COMMAND protobuf::protoc
        ARGS --cpp_out ${GENERATED_PROTOBUF_PATH}
             --proto_path ${CMAKE_CURRENT_SOURCE_DIR}/proto
             ${CMAKE_CURRENT_SOURCE_DIR}/proto/road_tile.proto
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/proto/road_tile.proto
        COMMENT "Running C++ protocol buffer compiler on ${CMAKE_CURRENT_SOURCE_DIR}/proto/road_tile.proto"
        VERBATIM)

add_custom_target(
        GenerateRoadTile ALL
        DEPENDS "${GENERATED_PROTOBUF_PATH}/road_tile.pb.cc"
                "${GENERATED_PROTOBUF_PATH}/road_tile.pb.h")

set(PROTO_SRCS "${GENERATED_PROTOBUF_PATH}/road_tile.pb.cc")
set(PROTO_HDRS "${GENERATED_PROTOBUF_PATH}/road_tile.pb.h")

add_library(road_tile STATIC ${PROTO_SRCS})
target_include_directories(road_tile PUBLIC ${GENERATED_PROTOBUF_PATH})
target_link_libraries(road_tile PUBLIC protobuf::libprotobuf)
add_dependencies(road_tile GenerateRoadTile)