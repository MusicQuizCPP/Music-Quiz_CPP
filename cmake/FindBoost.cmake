### Boost
FIND_PACKAGE(Boost COMPONENTS filesystem QUIET )
if ( NOT DEFINED Boost_FOUND OR NOT Boost_FOUND )
	message( FATAL_ERROR "The required boost libraries were not found." )
else ()
	INCLUDE_DIRECTORIES( SYSTEM ${Boost_INCLUDE_DIRS}) 
endif ()
