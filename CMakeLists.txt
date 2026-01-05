cmake_minimum_required(VERSION 3.16.3)

project(sp_vision)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)
message(STATUS "--------------------CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}--------------------")

find_package(OpenCV REQUIRED)
find_package(fmt REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(spdlog REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(nlohmann_json REQUIRED)
set(OpenVINO_DIR "/opt/intel/openvino_2024.6.0/runtime/cmake/")
find_package(OpenVINO REQUIRED)

include_directories(${EIGEN3_INCLUDE_DIR})
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR})

# 仅构建最小依赖：tools / io / auto_aim 子库
add_subdirectory(tools)
add_subdirectory(io)
add_subdirectory(tasks/auto_aim)

# 最小可执行：只保留装甲检测 + 传统解算 + 简单弹道的 demo
add_executable(armor_detect_and_solve tests/armor_detect_and_solve.cpp)

target_link_libraries(armor_detect_and_solve ${OpenCV_LIBS} fmt::fmt yaml-cpp tools io auto_aim)