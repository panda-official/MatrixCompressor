![example workflow](https://github.com/panda-official/MatrixCompressor/actions/workflows/ci.yml/badge.svg)

# MatrixCompressor

Compression library to compress sparse matrices

## Features

- Written in Modern C++
- Vectorization support both for x86 and arm
- Cross-platform

## Requirements

* CMake >= 3.16
* C++17 compiler
* conan >= 1.56

## Build and Installing


```
git clone https://github.com/panda-official/MatrixCompressor.git
mkdir build
cd build
conan install .. -s build_type=Release --build=missing
cmake .. -DCMAKE_TOOLCHAIN_FILE="conan_toolchain.cmake" -DCMAKE_POLICY_DEFAULT_CMP0091=NEW -DCMAKE_BUILD_TYPE=Release
cmake --build . -j --config Release
```

## Integration

### Using cmake target
```cmake
find_package(matrix_compressor REQUIRED)

add_executable(program program.cpp)
target_link_libraries(program matrix_compressor::matrix_compressor)
```

## References

- (streamvbyte)[https://github.com/victor1234/streamvbyte]
- (fpzip)[https://github.com/victor1234/fpzip]

