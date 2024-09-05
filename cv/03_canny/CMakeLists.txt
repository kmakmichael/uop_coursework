cmake_minimum_required(VERSION 3.10)
project(canny)
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
set(files
    ${CMAKE_CURRENT_SOURCE_DIR}/kernels.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/kernels.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/convolution.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/proc.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/proc.cpp
)
add_executable(canny canny.cpp ${files})
target_link_libraries(canny ${OpenCV_LIBS})
