cmake_minimum_required(VERSION 2.8)
project(xcapoc-resources)

#c++11 is required
# FIXME: Do compiler version check for c++11 support?
set(CMAKE_CXX_FLAGS "-std=c++11 -W -Wall")

set (APOCLIB_SOURCE_FILES
	apoclib/building.cpp
	apoclib/building.h
	apoclib/city_tile.cpp
	apoclib/city_tile.h
	apoclib/organisation.cpp
	apoclib/organisation.h
	apoclib/rect2d.h
	apoclib/voxel_layer.cpp
	apoclib/voxel_layer.h
)

find_package(PkgConfig)

pkg_check_modules(PC_TINYXML2 REQUIRED tinyxml2)

find_path(TINYXML2_INCLUDE_DIR tinyxml2.h
	HINTS ${PC_TINYXML2_INCLUDEDIR})

include_directories(${TINYXML2_INCLUDE_DIR})
add_library(apoclib ${APOCLIB_SOURCE_FILES})

target_link_libraries(apoclib ${PC_TINYXML2_LIBRARIES})

add_executable(load_buildings load_buildings.cpp)
target_link_libraries(load_buildings apoclib)

add_executable(load_citytiles load_citytiles.cpp)
target_link_libraries(load_citytiles apoclib)
