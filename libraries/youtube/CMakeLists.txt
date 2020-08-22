SET ( LIBYOUTUBE_FILES
      ${LIBYOUTUBE_FILES}
      ${CMAKE_CURRENT_SOURCE_DIR}/YoutubeUrlExtractor.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/HTTPSClient.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/StreamInfo.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/YoutubeDownloader.cpp
      CACHE INTERNAL ""
)

##Youtube library
add_library(youtube ${LIBYOUTUBE_FILES})
add_dependencies(youtube OpenSSL)
target_link_libraries(youtube OpenSSL:SSL OpenSSL:Crypto)

if(NOT MSVC)
    target_link_libraries(youtube dl)
endif()

ADD_SUBDIRECTORY(examples)