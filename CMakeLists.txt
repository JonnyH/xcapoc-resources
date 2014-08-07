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
	apoclib/citymap.cpp
	apoclib/citymap.h
	apoclib/palette.cpp
	apoclib/palette.h
	apoclib/palette_image.cpp
	apoclib/palette_image.h
	apoclib/pck.cpp
	apoclib/pck.h
)

find_package(PkgConfig)

pkg_check_modules(PC_TINYXML2 REQUIRED tinyxml2)
pkg_check_modules(PC_LIBPNG REQUIRED libpng)
pkg_check_modules(PC_SDL2 REQUIRED sdl2)
pkg_check_modules(PC_GLES2 REQUIRED glesv2)

find_path(TINYXML2_INCLUDE_DIR tinyxml2.h
	HINTS ${PC_TINYXML2_INCLUDEDIR})

find_path(PNGPP_INCLUDE_DIR png++/png.hpp)

include_directories(${TINYXML2_INCLUDE_DIR})
include_directories(${PNGPP_INCLUDE_DIR})

add_library(apoclib ${APOCLIB_SOURCE_FILES})

target_link_libraries(apoclib ${PC_TINYXML2_LIBRARIES})
target_link_libraries(apoclib ${PC_LIBPNG_LIBRARIES})

add_executable(load_buildings load_buildings.cpp)
target_link_libraries(load_buildings apoclib)

add_executable(load_citytiles load_citytiles.cpp)
target_link_libraries(load_citytiles apoclib)

add_executable(load_citymap load_citymap.cpp)
target_link_libraries(load_citymap apoclib)

add_executable(load_cityimages load_cityimages.cpp)
target_link_libraries(load_cityimages apoclib)

add_library(renderlib render/citymap_render.cpp)
target_link_libraries(renderlib apoclib)
target_link_libraries(renderlib ${PC_SDL2_LIBRARIES})
target_link_libraries(renderlib ${PC_GLES2_LIBRARIES})

add_executable(citymap_render_test citymap_render_test.cpp)
target_link_libraries(citymap_render_test apoclib)
target_link_libraries(citymap_render_test renderlib)
target_link_libraries(citymap_render_test ${PC_SDL2_LIBRARIES})

add_executable(bin2h bin2h.cpp)

add_executable(png2pal png2pal.c)
target_link_libraries(png2pal ${PC_LIBPNG_LIBRARIES})
