include(PothosConfigSIMD)

set(SourcesIn
    BCHCodec.cpp
    BCHDecoder.cpp
    BCHDecoderFactory.cpp)

PothosGenerateSIMDSources(
    Sources
    AFF3CTDynamic.json
    ${SourcesIn})

add_library(AFF3CTDynamic STATIC ${Sources})
target_link_libraries(AFF3CTDynamic aff3ct::aff3ct-shared-lib)
