cmake_minimum_required( VERSION 3.0.0 )

project( Opus )

include_directories( . )
include_directories( include )
include_directories( celt )
include_directories( silk )
include_directories( silk/float )
include_directories( silk/fixed )

add_definitions( -DHAVE_CONFIG_H=1 )

if( ${CMAKE_SYSTEM_NAME} STREQUAL Windows)
	add_definitions( -DUSE_ALLOCA=1 )
else()
	add_definitions( -DVAR_ARRAYS=1 )
endif()

set( CMAKE_BUILD_TYPE Release )

add_subdirectory( celt )
add_subdirectory( silk )
add_subdirectory( src )

add_subdirectory( tests )
