cmake_minimum_required( VERSION 3.0.0 )

project( Opus_Tests )

add_executable( test_opus_api test_opus_api.c )
target_link_libraries( test_opus_api opus )

add_executable( test_opus_decode test_opus_decode.c )
target_link_libraries( test_opus_decode opus )

add_executable( test_opus_encode opus_encode_regressions.c test_opus_encode.c )
target_link_libraries( test_opus_encode opus )

add_executable( test_opus_padding test_opus_padding.c )
target_link_libraries( test_opus_padding opus )
