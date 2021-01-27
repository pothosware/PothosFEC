set(Sources EntryPoints.cpp)

include(PothosConfigSIMD OPTIONAL RESULT_VARIABLE SIMD_SUPPORTED)

if(SIMD_SUPPORTED)
    set(SIMDSourcesIn
        BCHCodec.cpp
        BCHDecoder.cpp
        BCHDecoderFactory.cpp
        LDPCCodec.cpp
        LDPCDecoderBPHorizontalLayeredONMSInter.cpp
        LDPCDecoderFactory.cpp
        LDPCEncoderFactory.cpp
        LDPCEncoderFromQC.cpp
        SIMDEntryPoints.cpp)

    PothosGenerateSIMDSources(
        SIMDSources
        AFF3CTDynamic.json
        ${SIMDSourcesIn})
    list(APPEND Sources ${SIMDSources})

    include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR})

    add_definitions(-DAFF3CT_SIMD_SUPPORTED)
endif()

add_library(AFF3CTDynamic STATIC ${Sources})
target_link_libraries(AFF3CTDynamic aff3ct::aff3ct-shared-lib)
set_property(TARGET AFF3CTDynamic PROPERTY POSITION_INDEPENDENT_CODE TRUE)

if(SIMD_SUPPORTED)
    add_dependencies(AFF3CTDynamic AFF3CTDynamic_SIMDDispatcher)
endif()
