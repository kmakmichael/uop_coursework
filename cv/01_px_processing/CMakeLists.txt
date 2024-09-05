cmake_minimum_required(VERSION 3.10)
project(px_processing)
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
set(funcs
    ${CMAKE_CURRENT_SOURCE_DIR}/functions/pixelfuncs.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/functions/histeq.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/functions/thresholding.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/functions/morpho.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/functions/floodfill.cpp
)
add_executable(px_processing px_processing.cpp ${funcs})
target_link_libraries(px_processing ${OpenCV_LIBS})