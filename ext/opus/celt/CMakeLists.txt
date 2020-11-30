cmake_minimum_required( VERSION 3.0.0 )

project( Opus_CELT )

set( CELT_SOURCES bands.c
                  celt.c
                  celt_encoder.c
                  celt_decoder.c
                  cwrs.c
                  entcode.c
                  entdec.c
                  entenc.c
                  kiss_fft.c
                  laplace.c
                  mathops.c
                  mdct.c
                  modes.c
                  pitch.c
                  celt_lpc.c
                  quant_bands.c
                  rate.c
                  vq.c )

set( CELT_SOURCES_SSE x86/x86cpu.c
                      x86/x86_celt_map.c
                      x86/pitch_sse.c )

set( CELT_SOURCES_SSE2 x86/pitch_sse2.c )

set( CELT_SOURCES_SSE4_1 x86/celt_lpc_sse.c
                         x86/pitch_sse4_1.c )

set( CELT_SOURCES_ARM = arm/armcpu.c
                        arm/arm_celt_map.c )
set( CELT_SOURCES_ARM_ASM = arm/celt_pitch_xcorr_arm.s )
set( CELT_AM_SOURCES_ARM_ASM = arm/armopts.s.in )
set( CELT_SOURCES_ARM_NEON_INTR = arm/celt_neon_intr.c )

add_library( celt STATIC
                  ${CELT_SOURCES}
                )
