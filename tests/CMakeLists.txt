enable_testing()
include(CTest)
include(Catch)

add_executable(unit_tests matrix_compressor_test.cpp)
target_link_libraries(unit_tests ${MC_TARGET_NAME})
target_link_libraries(unit_tests Catch2::Catch2WithMain)

# Discover tests
catch_discover_tests(unit_tests)
