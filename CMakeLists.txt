cmake_minimum_required(VERSION 3.17)
project(Voronoi)

set(CMAKE_CXX_STANDARD 20)


find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(deps)
include_directories(libs)

add_executable(VoronoiBrut voronoi-brut.cpp)
add_executable(VoronoiSequential voronoi-sequential.cpp)
add_executable(VoronoiKMeans voronoi-k-means.cpp)

target_link_libraries(VoronoiBrut ${OpenCV_LIBS})
target_link_libraries(VoronoiSequential ${OpenCV_LIBS})
target_link_libraries(VoronoiKMeans ${OpenCV_LIBS})