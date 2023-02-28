cmake_minimum_required(VERSION 3.16)

project(matrix_compressor VERSION 0.1.0)

# Options
option(MC_BUILD_TESTS "Build unit tests" OFF)
option(MC_BUILD_EXAMPLES "Build examples" OFF)
option(MC_BUILD_BENCHMARKS "Build benchmarks" OFF)

find_package(blaze REQUIRED)
find_package(Catch2 3 REQUIRED)

# Dependencies
include(FetchContent)

fetchcontent_declare(
    streamvbyte
    GIT_REPOSITORY https://github.com/victor1234/streamvbyte.git
    GIT_TAG reduce-libs-number
)

fetchcontent_declare(
    fpzip
    GIT_REPOSITORY https://github.com/victor1234/fpzip.git
    GIT_TAG drift-637
)

fetchcontent_makeavailable(streamvbyte)
fetchcontent_makeavailable(fpzip)

# Create target
set(MC_TARGET_NAME ${PROJECT_NAME})
add_library(${MC_TARGET_NAME} sources/matrix_compressor.cpp)

# Add alias
add_library(${MC_TARGET_NAME}::${MC_TARGET_NAME} ALIAS ${MC_TARGET_NAME})

# Set fPIC
set_target_properties(${MC_TARGET_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
set_target_properties(streamvbyte PROPERTIES POSITION_INDEPENDENT_CODE ON)
set_target_properties(fpzip PROPERTIES POSITION_INDEPENDENT_CODE ON)

# Set compiler features
target_compile_features(${MC_TARGET_NAME} PUBLIC cxx_std_17)
set_target_properties(${MC_TARGET_NAME} PROPERTIES CXX_STANDARD_REQUIRED ON)

# Include paths
target_include_directories(
    ${MC_TARGET_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# Link dependencies
target_link_libraries(${MC_TARGET_NAME} PUBLIC blaze::blaze)
target_link_libraries(${MC_TARGET_NAME} PRIVATE streamvbyte)
target_link_libraries(${MC_TARGET_NAME} PRIVATE fpzip)

if(MC_BUILD_EXAMPLES)
    message(STATUS "Examples enabled")
    add_subdirectory(examples)
endif()

include(CTest)
if(MC_BUILD_TESTS)
    message(STATUS "Unit tests enabled")
    add_subdirectory(tests)
endif()

if(MC_BUILD_BENCHMARKS)
    message(STATUS "Benchmarks enabled")
    add_subdirectory(benchmarks)
endif()

# Install rules
include(GNUInstallDirs)

# Create package targets file
install(TARGETS ${MC_TARGET_NAME} streamvbyte EXPORT ${MC_TARGET_NAME}-target)
install(
    EXPORT ${MC_TARGET_NAME}-target
    FILE ${MC_TARGET_NAME}-targets.cmake
    NAMESPACE ${MC_TARGET_NAME}::
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake
)

include(CMakePackageConfigHelpers)

# Create package config file
configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/cmake/${MC_TARGET_NAME}-config.cmake.in"
    "${MC_TARGET_NAME}-config.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake
)
# Create package version file
write_basic_package_version_file(
    ${MC_TARGET_NAME}-config-version.cmake
    COMPATIBILITY ExactVersion
)

# Install cmake files
install(
    FILES
        "${PROJECT_BINARY_DIR}/${MC_TARGET_NAME}-config.cmake"
        "${PROJECT_BINARY_DIR}/${MC_TARGET_NAME}-config-version.cmake"
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake
)

# Install headers
install(
    DIRECTORY ${PROJECT_SOURCE_DIR}/matrix_compressor
    TYPE INCLUDE
    PATTERN "*.md"
    EXCLUDE
)
