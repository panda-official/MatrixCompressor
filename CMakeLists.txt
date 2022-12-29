cmake_minimum_required(VERSION 3.16)

project(matrix_compressor)

find_package(blaze REQUIRED)
find_package(Catch2 REQUIRED)

# Dependencies
include(FetchContent)

fetchcontent_declare(
    streamvbyte
    GIT_REPOSITORY https://github.com/lemire/streamvbyte.git
)

fetchcontent_declare(fpzip GIT_REPOSITORY https://github.com/LLNL/fpzip.git)

fetchcontent_makeavailable(streamvbyte)
fetchcontent_makeavailable(fpzip)

set(MC_TARGET_NAME ${PROJECT_NAME})
add_library(${MC_TARGET_NAME} sources/matrix_compressor.cpp)

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
target_link_libraries(${MC_TARGET_NAME} blaze::blaze)
target_link_libraries(${MC_TARGET_NAME} streamvbyte_static)
target_link_libraries(${MC_TARGET_NAME} fpzip)

add_subdirectory(examples)
add_subdirectory(tests)
