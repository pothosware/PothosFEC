########################################################################
# Project setup
########################################################################
cmake_minimum_required(VERSION 3.3)
project(PothosFEC C CXX)

find_package(Pothos 0.7 CONFIG REQUIRED)

list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/cmake)

########################################################################
# Find TurboFEC
########################################################################
find_package(TurboFEC REQUIRED)

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
    ${JSON_HPP_INCLUDE_DIR}
    ${TURBOFEC_INCLUDE_DIRS})

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
        Source/errnoname.c
        Source/GenericConvolution.cpp
        Source/LTETurboDecoder.cpp
        Source/LTETurboEncoder.cpp
        Source/Utility.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/ModuleInfo.cpp

        Testing/TestBitErrorRate.cpp
        Testing/TestConvolution.cpp
        Testing/TestLTETurboCoders.cpp
        Testing/TestModuleInfo.cpp
        Testing/TestUtility.cpp
    LIBRARIES
        ${TURBOFEC_LIBRARIES}
    ENABLE_DOCS ON
    DESTINATION fec
)
