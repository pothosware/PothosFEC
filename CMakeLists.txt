########################################################################
# Project setup
########################################################################
cmake_minimum_required(VERSION 3.3)
project(PothosFEC C CXX)

find_package(Pothos 0.7 CONFIG REQUIRED)

list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/cmake)

if(MSVC)
    message(STATUS "This component is currently incompatible with MSVC. Exiting...")
    return()
endif()

########################################################################
# Find dependencies
########################################################################
find_package(Threads)
find_package(aff3ct 2.3.5 EXACT REQUIRED)

########################################################################
# Build AFF3CT dynamic dispatcher
########################################################################
add_subdirectory(AFF3CTDynamic)

########################################################################
# Find or build TurboFEC
########################################################################
find_package(TurboFEC)
if(NOT TURBOFEC_FOUND)
    message(STATUS "TurboFEC not found. Building in-tree static library.")
    include(BuildTurboFEC)
endif()

########################################################################
# json.hpp header
########################################################################
find_path(JSON_HPP_INCLUDE_DIR NAMES json.hpp PATH_SUFFIXES nlohmann)

# TODO: skip, don't error
if (NOT JSON_HPP_INCLUDE_DIR)
    message(FATAL_ERROR "Pothos FEC toolkit requires json.hpp")
endif (NOT JSON_HPP_INCLUDE_DIR)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Source
    ${CMAKE_CURRENT_SOURCE_DIR}/Testing
    ${CMAKE_CURRENT_BINARY_DIR}
    ${TURBOFEC_INCLUDE_DIRS}
    ${JSON_HPP_INCLUDE_DIR})

configure_file(
    Source/ModuleInfo.cpp.in
    ${CMAKE_CURRENT_BINARY_DIR}/ModuleInfo.cpp)

include(PothosUtil)
POTHOS_MODULE_UTIL(
    TARGET FECBlocks
    SOURCES
        Source/BitErrorRate.cpp
        Source/ConvCodes.c
        Source/Convolution.cpp
        Source/ConvolutionBase.cpp
        Source/ConvolutionDocs.cpp
        Source/GenericConvolution.cpp
        Source/LTETurboDecoder.cpp
        Source/LTETurboEncoder.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/ModuleInfo.cpp
        ${SIMDSources}

        Source/AFF3CT/BCHCoders.cpp
        Source/AFF3CT/CoderBase.cpp
        Source/AFF3CT/LDPCCoders.cpp
        Source/AFF3CT/RACoders.cpp

        Testing/CoderTests.cpp
        Testing/TestAFF3CTCoders.cpp
        Testing/TestBitErrorRate.cpp
        Testing/TestConvolution.cpp
        Testing/TestLTETurboCoders.cpp
        Testing/TestModuleInfo.cpp
        Testing/TestUtility.cpp
    LIBRARIES
        ${TURBOFEC_LIBRARIES}
        AFF3CTDynamic
        aff3ct::aff3ct-shared-lib
    ENABLE_DOCS ON
    DESTINATION fec
)
if(NOT TURBOFEC_FOUND)
    add_dependencies(FECBlocks prj_turbofec)
endif()
