cmake_minimum_required( VERSION 3.0.0 )

project( Opus )

set( OPUS_SOURCES opus.c
                  opus_decoder.c
                  opus_encoder.c
                  opus_multistream.c
                  opus_multistream_encoder.c
                  opus_multistream_decoder.c
                  repacketizer.c )

set( OPUS_SOURCES_FLOAT analysis.c
                        mlp.c
                        mlp_data.c )

add_library( opus STATIC
                  ${OPUS_SOURCES}
                  ${OPUS_SOURCES_FLOAT} )
target_link_libraries( opus celt silk )

add_executable( opus_demo opus_demo.c )
target_link_libraries( opus_demo opus )
