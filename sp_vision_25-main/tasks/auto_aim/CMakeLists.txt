cmake_minimum_required(VERSION 3.16)

# OpenVINO路径，如果未设置则使用默认路径
if(NOT DEFINED OpenVINO_DIR)
    set(OpenVINO_DIR "/opt/intel/openvino_2024.6.0/runtime/cmake")
endif()

find_package(OpenVINO REQUIRED COMPONENTS Runtime)

add_library(auto_aim OBJECT 
    armor.cpp
    classifier.cpp 
    detector.cpp
    solver.cpp
    target.cpp
    tracker.cpp
)

target_link_libraries(auto_aim io openvino::runtime)