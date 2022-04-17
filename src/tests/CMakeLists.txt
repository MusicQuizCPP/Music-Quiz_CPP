# Target: test_helloWorld
add_executable(test_file "test_file.cpp")
add_dependencies(test_file ${PROJECT_NAME})
target_link_libraries(test_file ${PROJECT_NAME})

# Target: test_lightControl
if ( BUILD_LIGHT_CONTROL )
	add_executable(test_lightControl "test_lightControl.cpp")
	add_dependencies(test_lightControl ${PROJECT_NAME})
	target_link_libraries(test_lightControl ${PROJECT_NAME})
endif()