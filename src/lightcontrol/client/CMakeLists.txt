SET ( SRC_FILES
        ${SRC_FILES}
        ${CMAKE_CURRENT_SOURCE_DIR}/LightControlClient.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/DeviceState.cpp
        CACHE INTERNAL ""
)

ADD_SUBDIRECTORY(messages)
