 #*************************************************************************
 #   CMakeLists.txt  --  This file is part of Netris.                     *
 #                                                                        *
 #   Copyright (C) 2016, Ivo Filot                                        *
 #                                                                        *
 #   Netris is free software: you can redistribute it and/or modify       *
 #   it under the terms of the GNU General Public License as published    *
 #   by the Free Software Foundation, either version 3 of the License,    *
 #   or (at your option) any later version.                               *
 #                                                                        *
 #   Netris is distributed in the hope that it will be useful,            *
 #   but WITHOUT ANY WARRANTY; without even the implied warranty          *
 #   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.              *
 #   See the GNU General Public License for more details.                 *
 #                                                                        *
 #   You should have received a copy of the GNU General Public License    *
 #   along with this program.  If not, see http://www.gnu.org/licenses/.  *
 #                                                                        *
 #*************************************************************************/

# set minimum cmake requirements
cmake_minimum_required(VERSION 2.8)
project (quadtree)

# add custom directory to look for .cmake files
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/modules )

# Enable release build
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to 'Release' as none was specified.")
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# add OS specific
if(APPLE)
    add_definitions(-D_APPLE)
    add_definitions(-DEIGEN_NO_DEBUG)
    SET(BOOST_INCLUDEDIR "/opt/local/include")
    SET(BOOST_LIBRARYDIR "/opt/local/lib")
else()
    add_definitions(-DEIGEN_NO_DEBUG -Wno-literal-suffix)
    SET(BOOST_INCLUDEDIR "/usr/include")
    SET(BOOST_LIBRARYDIR "/usr/lib/x86_64-linux-gnu")
endif()

# set Boost
set (Boost_NO_SYSTEM_PATHS ON)
set (Boost_USE_MULTITHREADED ON)
set (Boost_USE_STATIC_LIBS ON)
set (Boost_USE_STATIC_RUNTIME OFF)
set (BOOST_ALL_DYN_LINK OFF)

# Include libraries
find_package(OpenAL REQUIRED)
find_package(glfw3 3.1 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(Boost COMPONENTS chrono regex system serialization filesystem log thread REQUIRED)
pkg_check_modules(EIGEN eigen3 REQUIRED)
pkg_check_modules(GLEW REQUIRED glew)
pkg_check_modules(PNG REQUIRED libpng)
pkg_check_modules(FREETYPE REQUIRED freetype2)
pkg_check_modules(ALUT REQUIRED freealut)
pkg_check_modules(OGG REQUIRED ogg)
pkg_check_modules(VORBIS REQUIRED vorbis)
pkg_check_modules(VORBISFILE REQUIRED vorbisfile)

# Set include folders
include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_BINARY_DIR}
                    ${Boost_INCLUDE_DIRS}
                    ${EIGEN_INCLUDE_DIRS}
                    ${FREETYPE_INCLUDE_DIRS}
                    ${OPENAL_INCLUDE_DIR}
                    ${ALUT_INCLUDE_DIR}
                    ${OGG_INCLUDE_DIR}
                    ${VORBIS_INCLUDE_DIR}
                    ${VORBISFILE_INCLUDE_DIR})

# Add sources
file(GLOB_RECURSE SOURCES "*.cpp")
add_executable(quadtree ${SOURCES})

# Set C++11
add_definitions(-std=c++11)

# Link libraries
if(UNIX AND NOT APPLE)
    SET(CMAKE_EXE_LINKER_FLAGS "-Wl,-rpath=\$ORIGIN/lib")
endif()
if(APPLE)
    SET(CMAKE_MACOSX_RPATH TRUE)
    SET_TARGET_PROPERTIES(quadtree PROPERTIES INSTALL_RPATH "@executable_path/lib")
    SET(CMAKE_EXE_LINKER_FLAGS "-L${GLEW_LIBRARY_DIRS}")
    ADD_LIBRARY(lib_png STATIC IMPORTED)
    SET_TARGET_PROPERTIES(lib_png PROPERTIES IMPORTED_LOCATION ${PNG_STATIC_LIBRARY_DIRS}/lib${PNG_LIBRARIES}.a)
    ADD_LIBRARY(lib_freetype STATIC IMPORTED)
    SET_TARGET_PROPERTIES(lib_freetype PROPERTIES IMPORTED_LOCATION ${FREETYPE_STATIC_LIBRARY_DIRS}/libfreetype.a)
    ADD_LIBRARY(lib_z STATIC IMPORTED)
    SET_TARGET_PROPERTIES(lib_z PROPERTIES IMPORTED_LOCATION ${FREETYPE_STATIC_LIBRARY_DIRS}/libz.a)
    ADD_LIBRARY(lib_bz2 STATIC IMPORTED)
    SET_TARGET_PROPERTIES(lib_bz2 PROPERTIES IMPORTED_LOCATION ${FREETYPE_STATIC_LIBRARY_DIRS}/libbz2.a)
    ADD_LIBRARY(lib_glew STATIC IMPORTED)
    SET_TARGET_PROPERTIES(lib_glew PROPERTIES IMPORTED_LOCATION ${GLEW_STATIC_LIBRARY_DIRS}/lib${GLEW_STATIC_LIBRARIES}.a)
    target_link_libraries(quadtree glfw ${VORBISFILE_LIBRARIES} ${VORBIS_LIBRARIES} ${OGG_LIBRARIES} ${ALUT_LIBRARIES} ${GLFW3_LIBRARY} ${X11_Xinerama_LIB} ${X11_Xrandr_LIB} ${X11_Xcursor_LIB} ${OPENGL_glu_LIBRARY} ${Boost_LIBRARIES} lib_png lib_freetype lib_z lib_bz2 lib_glew pthread dl)
else()
    target_link_libraries(quadtree glfw ${VORBISFILE_LIBRARIES} ${VORBIS_LIBRARIES} ${OGG_LIBRARIES} ${ALUT_LIBRARIES} ${GLFW3_LIBRARY} ${X11_Xinerama_LIB} ${X11_Xrandr_LIB} ${X11_Xcursor_LIB} ${OPENGL_glu_LIBRARY} ${GLEW_STATIC_LIBRARIES} ${Boost_LIBRARIES} ${PNG_LIBRARIES} ${FREETYPE_LIBRARIES} ${OPENAL_LIBRARY} pthread dl)
endif()

# add Boost definition
add_definitions(-DBOOST_LOG_DYN_LINK)

# add Wno-literal-suffix to suppress warning messages
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS}")

###
# Installing
##
install (TARGETS quadtree DESTINATION bin)
