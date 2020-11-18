cmake_minimum_required(VERSION 3.17)
project(Voronoi)

set(CMAKE_CXX_STANDARD 20)


find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(deps)

add_executable(VoronoiBrut voronoi-brut.cpp)

target_link_libraries(VoronoiBrut ${OpenCV_LIBS})
