cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
cmake_policy(SET CMP0074 NEW)

include(CheckCCompilerFlag)
include(CheckFunctionExists)
include(CheckLibraryExists)
include(CheckSymbolExists)
include(CMakePackageConfigHelpers)
include(ExternalProject)


project(MusicQuiz)

### Options
OPTION(BUILD_TESTS "Build tests" ON)

### Set ROOT
SET(ROOT ${CMAKE_CURRENT_SOURCE_DIR})

SET(CMAKE_AUTOMOC ON)

### Set build type
IF( NOT CMAKE_BUILD_TYPE )
  SET(CMAKE_BUILD_TYPE release)
ENDIF()
MESSAGE("-- Build type: " ${CMAKE_BUILD_TYPE})

### Set the output dir for generated libraries and binaries
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin" CACHE PATH "Runtime directory" FORCE)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/libs" CACHE PATH "Library directory" FORCE)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/libs" CACHE PATH "Archive directory" FORCE)

### add extra definitions. c++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)

if( MSVC )
    # Using c++17.
    # -permissive- Forces C++ standards in the code.
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /permissive- ")
    add_compile_definitions(BOOST_ASIO_DISABLE_CONCEPTS)
    add_compile_definitions(_WIN32_WINNT=0x0A00) #Set target to WIN10
    add_compile_definitions(WIN32_LEAN_AND_MEAN) #Disable complains about winsock.h includes
    add_compile_definitions(BOOST_ALL_NO_LIB)    #Don't autolink boost libraries
    add_compile_definitions(_SILENCE_CXX17_ALLOCATOR_VOID_DEPRECATION_WARNING) #shut up MSVC
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)                           #shut up MSVC

else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wconversion -Wlogical-op -Werror -Wpointer-arith -Wcast-qual -Wall -Wextra -Wshadow -Wold-style-cast -Wnon-virtual-dtor -Wcast-align -Woverloaded-virtual -pthread -g")
endif()


### Find Qt
include("cmake/FindQt.cmake")
find_package(Qt5Multimedia REQUIRED)
find_package(Qt5MultimediaWidgets REQUIRED)

### Boost
include("cmake/FindBoost.cmake")

INCLUDE_DIRECTORIES( src )

### Zeroconf
include_directories(SYSTEM submodules/mdns/)

### cereal serialisation library
include_directories(SYSTEM submodules/cereal/include)

### JSON
include_directories(SYSTEM submodules/json/include)

### LibreSSL
include("cmake/StaticOpenSSL.cmake")

#Add libraries
include_directories(libraries)
ADD_SUBDIRECTORY(libraries)


### Main project sources
SET(SRC_FILES CACHE INTERNAL "" FORCE)
ADD_SUBDIRECTORY(src)


### add the library
add_library(${PROJECT_NAME} ${SRC_FILES})

target_link_libraries(${PROJECT_NAME} 
                      ${Qt5Core_QTMAIN_LIBRARIES}
                      ${Qt5Core_LIBRARIES}
                      ${Qt5Gui_LIBRARIES} 
                      ${Qt5Widgets_LIBRARIES} 
                      ${Qt5OpenGL_LIBRARIES} 
                      ${Qt5Multimedia_LIBRARIES} 
                      ${Qt5MultimediaWidgets_LIBRARIES})