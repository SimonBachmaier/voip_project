cmake_minimum_required( VERSION 3.0 )

project( VoIP_Project )

set( BUILD_TESTING OFF CACHE BOOL "Build tests (includes tests in all subprojects)." )
set( VOIP_BUILD_OPUS ON CACHE BOOL "Build opus audio coding library." )

link_libraries(m)

include(CheckCXXCompilerFlag)

CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)

if(${COMPILER_SUPPORTS_CXX11})
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
	if(${CMAKE_CXX_COMPILER_ID} STREQUAL "AppleClang")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
	endif()
endif()

set( VOIP_EXT_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ext/tclap/include ${CMAKE_CURRENT_SOURCE_DIR}/ext/rtaudio )


if( ${VOIP_BUILD_OPUS} )
	add_subdirectory( ext/opus )
	list( APPEND VOIP_EXT_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ext/opus/include )
endif()

if( ${CMAKE_SYSTEM_NAME} STREQUAL Windows )
  set( AUDIO_WINDOWS_WASAPI ON CACHE BOOL "Set WASAPI audio driver for Windows." )
elseif( ${CMAKE_SYSTEM_NAME} STREQUAL Linux )
  set( AUDIO_LINUX_PULSE ON CACHE BOOL "Set ALSA audio driver for Linux." )
elseif( ${CMAKE_SYSTEM_NAME} STREQUAL Darwin )
  set( AUDIO_OSX_CORE ON CACHE BOOL "Set CoreAudio audio driver for OSX." )
else()
  message( FATAL_ERROR "Unknown system." )
endif()

add_subdirectory( ext/rtaudio )

#Todo: fix linux build
if( ${CMAKE_SYSTEM_NAME} STREQUAL Windows)
	set( VOIP_EXT_LIBS rtaudio_static opus ws2_32)
else()
	set( VOIP_EXT_LIBS rtaudio opus)
endif()

add_subdirectory( src )