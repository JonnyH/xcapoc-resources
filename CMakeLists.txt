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

find_path(TINYXML2_INCLUDE_DIR tinyxml2.h
	HINTS ${PC_TINYXML2_INCLUDEDIR})

find_path(PNGPP_INCLUDE_DIR png++/png.hpp)

foreach(LIBRARY ${PC_TINYXML2_LIBRARIES})
	find_library(LIBRARY_PATH-${LIBRARY} NAMES ${LIBRARY} PATHS ${PC_TINYXML2_LIBRARY_DIRS})
	if (LIBRARY_PATH-${LIBRARY}-NOTFOUND)
		message(FATAL_ERROR "Couldn't find library \"${LIBRARY}\" in ${PC_TINYXML2_LIBRARY_DIRS}")
	endif()
	list(APPEND TINYXML2_LIBRARIES ${LIBRARY_PATH-${LIBRARY}})
endforeach()

foreach(LIBRARY ${PC_LIBPNG_LIBRARIES})
	find_library(LIBRARY_PATH-${LIBRARY} NAMES ${LIBRARY} PATHS ${PC_LIBPNG_LIBRARY_DIRS})
	if (LIBRARY_PATH-${LIBRARY}-NOTFOUND)
		message(FATAL_ERROR "Couldn't find library \"${LIBRARY}\" in ${PC_LIBPNG_LIBRARY_DIRS}")
	endif()
	list(APPEND LIBPNG_LIBRARIES ${LIBRARY_PATH-${LIBRARY}})
endforeach()

set(TINYXML2_LIBRARY ${TINYXML2_LIBRARIES} CACHE FILEPATH "tinyxml2 library path")
set(LIBPNG_LIBRARY ${LIBPNG_LIBRARIES} CACHE FILEPATH "libpng library path")

include_directories(${TINYXML2_INCLUDE_DIR})
include_directories(${PNGPP_INCLUDE_DIR})

add_library(apoclib ${APOCLIB_SOURCE_FILES})

target_link_libraries(apoclib ${TINYXML2_LIBRARY})
target_link_libraries(apoclib ${LIBPNG_LIBRARY})

add_executable(load_buildings load_buildings.cpp)
target_link_libraries(load_buildings apoclib)

add_executable(load_citytiles load_citytiles.cpp)
target_link_libraries(load_citytiles apoclib)

add_executable(load_citymap load_citymap.cpp)
target_link_libraries(load_citymap apoclib)

add_executable(pck2png pck2png.cpp)
target_link_libraries(pck2png apoclib)

add_executable(bin2h bin2h.cpp)

add_executable(png2pal png2pal.c)
target_link_libraries(png2pal ${LIBPNG_LIBRARY})

add_executable(pcx2pal pcx2pal.c)

add_executable(font2png font2png.cpp)
target_link_libraries(font2png apoclib)


add_executable(split_tab split_tab.cpp tabfile.cpp)
