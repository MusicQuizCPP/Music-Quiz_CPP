### Find Qt ###
find_package(Qt5 COMPONENTS Core Widgets Gui OpenGL )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Core_QTMAIN_EXECUTABLE_COMPILE_FLAGS} ${Qt5Core_EXECUTABLE_COMPILE_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS} ${Qt5Gui_EXECUTABLE_COMPILE_FLAGS} ${Qt5OpenGL_EXECUTABLE_COMPILE_FLAGS}")
set(QT_INCLUDES ${Qt5Core_QTMAIN_INCLUDE_DIRS} ${Qt5Core_INCLUDE_DIRS} ${Qt5Gui_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS} ${Qt5OpenGL_INCLUDE_DIRS})

INCLUDE_DIRECTORIES(SYSTEM ${QT_INCLUDES}) 
link_directories(${Boost_LIBRARY_DIRS})
