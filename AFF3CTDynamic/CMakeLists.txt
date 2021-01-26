include(PothosConfigSIMD)

set(SourcesIn
    BCHCodec.cpp
    BCHDecoder.cpp
    BCHDecoderFactory.cpp
    EntryPoints.cpp
    LDPCEncoderFromQC.cpp
    LDPCMatrixHandler.cpp)

PothosGenerateSIMDSources(
    Sources
    AFF3CTDynamic.json
    ${SourcesIn})

add_library(AFF3CTDynamic STATIC ${Sources})
target_link_libraries(AFF3CTDynamic aff3ct::aff3ct-shared-lib)
add_dependencies(AFF3CTDynamic AFF3CTDynamic_SIMDDispatcher)
set_property(TARGET AFF3CTDynamic PROPERTY POSITION_INDEPENDENT_CODE TRUE)
