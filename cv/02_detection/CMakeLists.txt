cmake_minimum_required(VERSION 3.10)
project(detection)
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
set(pxfuncs
    ${CMAKE_CURRENT_SOURCE_DIR}/pxfuncs/pixelfuncs.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/pxfuncs/histeq.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/pxfuncs/thresholding.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/pxfuncs/morpho.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/pxfuncs/floodfill.cpp
)
set(algs
    ${CMAKE_CURRENT_SOURCE_DIR}/connected_components.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/connected_components.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/moment.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/moment.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/pca.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/pca.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/region.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/region.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wall.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wall.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/classification.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/classification.cpp
)
add_executable(detection detection.cpp ${pxfuncs} ${algs})
target_link_libraries(detection ${OpenCV_LIBS})
