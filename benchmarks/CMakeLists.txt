include(Catch)

add_executable(benchmarks matrix_compressor_benchmark.cpp)
target_link_libraries(benchmarks ${MC_TARGET_NAME})
target_link_libraries(benchmarks Catch2::Catch2WithMain)
