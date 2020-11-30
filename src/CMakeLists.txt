cmake_minimum_required( VERSION 3.0 )

project( VoIP_Project )

include_directories( ${VOIP_EXT_INC_DIR} )

set( SRC main.cpp
         voip_project.cpp
         audio_buffer.cpp
         audio_mixer.cpp
         soundcard_interaction.cpp
         playout_buffer.cpp
         sender.cpp
         receiver.cpp
         rtp_utils.cpp
         audio_encoder.cpp
         audio_decoder.cpp )

# Windows only
set( WIN_SRC windows_udp_socket.cpp )
# TODO Linux / OSX only
# set( UNIX_SRC unix_udp_socket.cpp )

set( HDR voip_project.h
         audio_buffer.h
         audio_mixer.h
         udp_socket.h
         soundcard_interaction.h
         playout_buffer.h
         sender.h
         receiver.h
         rtp_utils.h
         audio_encoder.h
         audio_decoder.h )

if( ${CMAKE_SYSTEM_NAME} STREQUAL Windows )
  list( APPEND SRC ${WIN_SRC} )
#else()
#  list( APPEND SRC ${UNIX_SRC} )
endif()

set( EXECUTABLE_NAME voip_project)

add_executable( ${EXECUTABLE_NAME} ${SRC} ${HDR})
target_link_libraries( ${EXECUTABLE_NAME} ${VOIP_EXT_LIBS} )
