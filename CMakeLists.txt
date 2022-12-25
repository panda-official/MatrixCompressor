cmake_minimum_required(VERSION 3.16)

project(matrix_compressor)

find_package(blaze REQUIRED)

set(MC_TAREGET_NAME ${PROJECT_NAME})
add_library(${MC_TAREGET_NAME} sources/matrix_compressor.cpp)

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
target_link_libraries(${WB_TARGET_NAME} blaze::blaze)

add_subdirectory(examples)
