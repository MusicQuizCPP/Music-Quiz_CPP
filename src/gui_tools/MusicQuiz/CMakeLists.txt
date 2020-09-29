
#Target: MusicQuiz
SET(UisFiles MusicQuizGUI.ui)
SET(RccSrcFiles)
QT5_ADD_RESOURCES(RccSrcFiles musicQuiz_resources.qrc)
QT5_WRAP_UI(UiHeaders ${UisFiles})

SET(CMAKE_INCLUDE_CURRENT_DIR ON)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

add_executable(MusicQuizGUI main.cpp MusicQuizController.cpp ${RccSrcFiles} ${UiHeaders})
add_dependencies(MusicQuizGUI ${PROJECT_NAME})
target_link_libraries(MusicQuizGUI ${PROJECT_NAME})

