set(turbofec_source_dir ${PROJECT_SOURCE_DIR}/turbofec)
set(turbofec_binary_dir ${PROJECT_BINARY_DIR}/turbofec)

include(ExternalProject)
ExternalProject_Add(
    prj_turbofec
    SOURCE_DIR ${turbofec_source_dir}
    CONFIGURE_COMMAND autoreconf -i ${turbofec_source_dir} && ${turbofec_source_dir}/configure --enable-shared=no --enable-static=yes --prefix=${turbofec_binary_dir}
    PREFIX ${turbofec_source_dir}
    BUILD_COMMAND make)

ExternalProject_Get_Property(prj_turbofec install_dir)

set(TURBOFEC_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/turbofec/include)
set(TURBOFEC_LIBRARIES ${PROJECT_BINARY_DIR}/turbofec/lib/libturbofec.a)

# Extract the version from the autoconf file
file(STRINGS ${turbofec_source_dir}/configure.ac turbofec_version REGEX "\\[[0-9]+\\.[0-9]+\\]")
string(REGEX MATCH "[0-9]+\\.[0-9]+" TURBOFEC_VERSION ${turbofec_version})
