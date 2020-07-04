########################################################################
# Project setup
########################################################################
cmake_minimum_required(VERSION 3.3)
project(PothosFEC C CXX)

find_package(Pothos 0.7 CONFIG REQUIRED)

list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/cmake)

########################################################################
# Find dependencies
########################################################################
find_package(Threads REQUIRED)
find_package(aff3ct REQUIRED)

########################################################################
# Find or build TurboFEC
########################################################################
find_package(TurboFEC)
if(NOT TURBOFEC_FOUND)
    message(STATUS "TurboFEC not found. Building in-tree static library.")

    set(turbofec_source_dir ${CMAKE_CURRENT_SOURCE_DIR}/turbofec)
    set(turbofec_binary_dir ${CMAKE_CURRENT_BINARY_DIR}/turbofec)

    include(ExternalProject)
    ExternalProject_Add(
        prj_turbofec
        SOURCE_DIR ${turbofec_source_dir}
        CONFIGURE_COMMAND autoreconf -i ${turbofec_source_dir} && ${turbofec_source_dir}/configure --enable-shared=no --enable-static=yes --prefix=${turbofec_binary_dir}
        PREFIX ${turbofec_source_dir}
        BUILD_COMMAND make)

    ExternalProject_Get_Property(prj_turbofec install_dir)

    set(TURBOFEC_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR}/turbofec/include)
    set(TURBOFEC_LIBRARIES ${CMAKE_CURRENT_BINARY_DIR}/turbofec/lib/libturbofec.a)
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
    ${TURBOFEC_INCLUDE_DIRS}
    ${JSON_HPP_INCLUDE_DIR})

configure_file(
    Source/ModuleInfo.cpp.in
    ${CMAKE_CURRENT_BINARY_DIR}/ModuleInfo.cpp)

include(PothosUtil)
POTHOS_MODULE_UTIL(
    TARGET FECBlocks
    SOURCES
        Source/AZCWEncoder.cpp
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
        aff3ct::aff3ct-shared-lib
    ENABLE_DOCS ON
    DESTINATION fec
)
if(NOT TURBOFEC_FOUND)
    add_dependencies(FECBlocks prj_turbofec)
endif()
