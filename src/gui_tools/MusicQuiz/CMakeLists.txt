
#Target: MusicQuiz
SET(CMAKE_INCLUDE_CURRENT_DIR ON)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

add_executable(MusicQuizGUI WIN32 main.cpp MusicQuizController.cpp MusicQuizGUI.ui musicQuiz_resources.qrc)
add_dependencies(MusicQuizGUI ${PROJECT_NAME})
target_link_libraries(MusicQuizGUI ${PROJECT_NAME})

