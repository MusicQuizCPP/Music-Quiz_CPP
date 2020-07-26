cmake_minimum_required(VERSION 2.8.11 FATAL_ERROR)

project(MusicQuiz)

### Options
OPTION(BUILD_TESTS "Build tests" ON)

### Set ROOT
SET(ROOT ${CMAKE_CURRENT_SOURCE_DIR})

SET(CMAKE_AUTOMOC ON)

### add extra definitions. c++17
set(CMAKE_CXX_STANDARD 17)
if( MSVC )
	# Using c++17.
	# -permissive- Forces C++ standards in the code.
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17 /permissive- ")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wconversion -Wlogical-op -Wpointer-arith -Wcast-qual -Wall -Wextra -Wshadow -Wold-style-cast -Wnon-virtual-dtor -Wcast-align -Woverloaded-virtual -pthread -g")
endif()

### Set build type
IF( NOT CMAKE_BUILD_TYPE )
  SET(CMAKE_BUILD_TYPE release)
ENDIF()
MESSAGE("-- Build type: " ${CMAKE_BUILD_TYPE})

### Find Qt
include("cmake/FindQt.cmake")
find_package(Qt5Multimedia REQUIRED)
find_package(Qt5MultimediaWidgets REQUIRED)

### Boost
include("cmake/FindBoost.cmake")

INCLUDE_DIRECTORIES( src )

### Zeroconf
include_directories(SYSTEM submodules/mdns/)

### Set the output dir for generated libraries and binaries
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin" CACHE PATH "Runtime directory" FORCE)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/libs" CACHE PATH "Library directory" FORCE)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/libs" CACHE PATH "Archive directory" FORCE)

### find the files
SET(SRC_FILES CACHE INTERNAL "" FORCE)
ADD_SUBDIRECTORY(src)

### add the library
add_library(${PROJECT_NAME} ${SRC_FILES})
target_link_libraries(${PROJECT_NAME} ${Qt5Core_QTMAIN_LIBRARIES} ${Qt5Core_LIBRARIES} ${Qt5Gui_LIBRARIES} ${Qt5Widgets_LIBRARIES} ${Qt5OpenGL_LIBRARIES} ${Qt5Multimedia_LIBRARIES} ${Qt5MultimediaWidgets_LIBRARIES})
