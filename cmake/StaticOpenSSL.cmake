include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/submodules/openssl/include)

if( MSVC )
    if("${CMAKE_GENERATOR}" MATCHES "(Win64|IA64)")
        SET(OPENSSL_CONFIG_CMD cd ${PROJECT_SOURCE_DIR}/submodules/openssl && perl Configure VC-WIN64A)
    else()
        SET(OPENSSL_CONFIG_CMD cd ${PROJECT_SOURCE_DIR}/submodules/openssl && perl Configure VC-WIN32)
    endif()

    ExternalProject_Add(OpenSSL
                        SOURCE_DIR ${PROJECT_SOURCE_DIR}/submodules/openssl
                        CONFIGURE_COMMAND ${OPENSSL_CONFIG_CMD}
                        INSTALL_COMMAND cmake -E echo "Skipping install step."
                        BUILD_COMMAND nmake
                        BUILD_IN_SOURCE 1 
                        )

else()
    SET(OPENSSL_CONFIG_CMD cd ${PROJECT_SOURCE_DIR}/submodules/openssl && ./config)

    ExternalProject_Add(OpenSSL
                        SOURCE_DIR ${PROJECT_SOURCE_DIR}/submodules/openssl
                        CONFIGURE_COMMAND ${OPENSSL_CONFIG_CMD}
                        INSTALL_COMMAND cmake -E echo "Skipping install step."
                        BUILD_IN_SOURCE 1 
                        )

endif()

ADD_LIBRARY(OpenSSL:Crypto STATIC IMPORTED)
SET_TARGET_PROPERTIES(OpenSSL:Crypto PROPERTIES
    IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/submodules/openssl/libcrypto${CMAKE_STATIC_LIBRARY_SUFFIX})

ADD_LIBRARY(OpenSSL:SSL STATIC IMPORTED)
SET_TARGET_PROPERTIES(OpenSSL:SSL PROPERTIES
    IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/submodules/openssl/libssl${CMAKE_STATIC_LIBRARY_SUFFIX})
