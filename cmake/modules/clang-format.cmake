# A CMake script to find all source files and setup clang-format targets for them

# Find all source files
set(CLANG_FORMAT_CXX_FILE_EXTENSIONS ${CLANG_FORMAT_CXX_FILE_EXTENSIONS} *.cpp *.h *.cxx *.hxx *.hpp *.cc *.ipp)

foreach (INCLUDE_PATTERN ${CLANG_FORMAT_INCLUDE_PATTERNS})
    foreach (EXTENSION ${CLANG_FORMAT_CXX_FILE_EXTENSIONS})
    file(GLOB_RECURSE INCLUDE_PATTERN_FILES ${INCLUDE_PATTERN}${EXTENSION})
    set(ALL_SOURCE_FILES ${ALL_SOURCE_FILES} ${INCLUDE_PATTERN_FILES})
    endforeach()
endforeach()

execute_process(COMMAND git diff --name-only origin/master
                OUTPUT_VARIABLE GIT_BRANCH_FILES
                ERROR_QUIET)


list(LENGTH GIT_BRANCH_FILES GIT_BRANCH_FILE_COUNT)
if(${GIT_BRANCH_FILE_COUNT})
    string(REPLACE "\r\n" ";" GIT_BRANCH_FILES ${GIT_BRANCH_FILES})
    string(REPLACE "\n" ";" GIT_BRANCH_FILES ${GIT_BRANCH_FILES})
endif()

set(GIT_BRANCH_SOURCE_FILES "")

foreach (GIT_BRANCH_FILE ${GIT_BRANCH_FILES})
    if(${PROJECT_SOURCE_DIR}/${GIT_BRANCH_FILE} IN_LIST ALL_SOURCE_FILES)
    set(GIT_BRANCH_SOURCE_FILES ${GIT_BRANCH_SOURCE_FILES} ${PROJECT_SOURCE_DIR}/${GIT_BRANCH_FILE})
    endif()
endforeach()

add_custom_target(format
    COMMENT "Running clang-format to change files"
    COMMAND ${CLANG_FORMAT_BIN_NAME}
    -style=file
    -i
    ${ALL_SOURCE_FILES}
)


add_custom_target(format-check
    COMMENT "Checking clang-format changes"
    # Use ! to negate the result for correct output
    COMMAND perl ${CMAKE_CURRENT_LIST_DIR}/../scripts/clang-format-check-files.pl \"${CLANG_FORMAT_BIN_NAME}\" ${ALL_SOURCE}
)

add_custom_target(format-branch
    COMMENT "Running clang-format to change files in branch"
    COMMAND ${CLANG_FORMAT_BIN_NAME}
    -style=file
    -i
    ${GIT_BRANCH_SOURCE_FILES}
)

add_custom_target(format-check-branch
    COMMENT "Checking clang-format changes in branch"
    # Use ! to negate the result for correct output
    COMMAND perl ${CMAKE_CURRENT_LIST_DIR}/../scripts/clang-format-check-files.pl \"${CLANG_FORMAT_BIN_NAME}\" ${GIT_BRANCH_SOURCE_FILES}
)