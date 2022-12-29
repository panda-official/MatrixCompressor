#include <matrix_compressor/matrix_compressor.h>

#include <iostream>

#include <catch2/catch_test_macros.hpp>

/**
 * Random data generator
 **/
class DataGenerator {
  std::default_random_engine random_engine_;
  std::uniform_real_distribution<float> distribution_{0, 1};

 public:
  /**
   * Create sparse vector with random data
   * @param size vector length
   * @param ratio non-zero elements ratio
   * @return sparse vector
   **/
  blaze::CompressedVector<float> GenerateSparseVector(size_t size, float ratio);

  /**
   * Create sparse matrix with random data
   * @param rows matrix rows
   * @param cols matrix columns
   * @param ratio non-zero elements ratio
   * @return sparse matrix
   **/
  blaze::DynamicMatrix<float> GenerateMatrix2d(size_t rows, size_t cols,
                                               float ratio);
};

blaze::CompressedVector<float> DataGenerator::GenerateSparseVector(
    size_t size, float ratio) {
  blaze::CompressedVector<float> vector(size);
  for (size_t i = 0; i < size; ++i) {
    if (distribution_(random_engine_) < ratio) {
      vector[i] = distribution_(random_engine_);
    }
  }
  return vector;
}

blaze::DynamicMatrix<float> DataGenerator::GenerateMatrix2d(size_t rows,
                                                            size_t cols,
                                                            float ratio) {
  blaze::DynamicMatrix<float> matrix(rows, cols);
  for (size_t i = 0; i < rows; ++i) {
    for (size_t j = 0; j < cols; ++j) {
      if (distribution_(random_engine_) < ratio) {
        matrix(i, j) = distribution_(random_engine_);
      }
    }
  }
  return matrix;
}

TEST_CASE("Compress and decompress vector", "[matrix_compressor]") {
  SECTION("Empty vector") {
    auto compressed = matrix_compressor::compress({});
    REQUIRE_FALSE(compressed.is_valid);
  }

  SECTION("Zeros vector") {
    auto compressed = matrix_compressor::compress({0, 0, 0, 0, 0});
    REQUIRE_FALSE(compressed.is_valid);
  }

  DataGenerator generator;
  auto vector = generator.GenerateSparseVector(100, 0.1);

  auto compressed = matrix_compressor::compress(vector);
  REQUIRE(compressed.is_valid);

  SECTION("Invalid compressed vector") {
    compressed.is_valid = false;
    auto decompressed = matrix_compressor::decompress(compressed);
    REQUIRE(decompressed.size() == 0);
  }

  auto decompressed = matrix_compressor::decompress(compressed);

  REQUIRE(vector == decompressed);
}
