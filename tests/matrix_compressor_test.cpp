// Copyright 2020-2022 PANDA GmbH

#include <matrix_compressor/matrix_compressor.h>

#include <iostream>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

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
  blaze::CompressedMatrix<float> GenerateSparseMatrix(size_t rows, size_t cols,
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

blaze::CompressedMatrix<float> DataGenerator::GenerateSparseMatrix(
    size_t rows, size_t cols, float ratio) {
  blaze::DynamicMatrix<float> matrix = blaze::zero<float>(rows, cols);
  for (size_t i = 0; i < rows; ++i) {
    for (size_t j = 0; j < cols; ++j) {
      if (distribution_(random_engine_) < ratio) {
        matrix(i, j) = distribution_(random_engine_);
      }
    }
  }
  return matrix;
}

TEST_CASE("BlazeCompressor::Compress()", "[vector]") {
  SECTION("Empty vector") {
    auto compressed = matrix_compressor::BlazeCompressor().Compress(
        blaze::CompressedVector<float>{});
    REQUIRE_FALSE(compressed.is_valid);
  }

  SECTION("Zeros vector") {
    auto compressed =
        matrix_compressor::BlazeCompressor().Compress({0, 0, 0, 0, 0});
    REQUIRE_FALSE(compressed.is_valid);
  }

  SECTION("Random vector") {
    DataGenerator generator;
    auto vector = generator.GenerateSparseVector(1000, 0.1);
    auto compressed = matrix_compressor::BlazeCompressor().Compress(vector);
    REQUIRE(compressed.is_valid);
  }
}

TEST_CASE("BlazeCompressor::Decompress()", "[vector]") {
  SECTION("Invalid compressed vector") {
    matrix_compressor::CompressedVector compressed;
    compressed.is_valid = false;
    auto decompressed =
        matrix_compressor::BlazeCompressor().Decompress(compressed);
    REQUIRE(decompressed.size() == 0);
  }
}

TEST_CASE("Compress and decompress vector", "[matrix_compressor]") {
  DataGenerator generator;
  auto vector = generator.GenerateSparseVector(100, 0.1);

  auto compressed = matrix_compressor::BlazeCompressor().Compress(vector);
  REQUIRE(compressed.is_valid);

  CAPTURE(compressed.indexes.size());
  for (auto i = 0; i < compressed.indexes.size(); i++) {
    CAPTURE(compressed.indexes[i]);
  }

  SECTION("direct") {
    auto decompressed =
        matrix_compressor::BlazeCompressor().Decompress(compressed);

    REQUIRE(vector == decompressed);
  }
}

TEST_CASE("BlazeCompressor::Compress()", "[matrix]") {
  SECTION("Empty matrix") {
    auto compressor = matrix_compressor::BlazeCompressor();
    REQUIRE_THROWS_AS(compressor.Compress(blaze::CompressedMatrix<float>{}),
                      std::invalid_argument);
  }

  SECTION("Random matrix") {
    DataGenerator generator;
    auto matrix = generator.GenerateSparseMatrix(100, 100, 0.1);
    auto compressed = matrix_compressor::BlazeCompressor().Compress(matrix);
    REQUIRE(compressed.is_valid);
  }
}

TEST_CASE("BlazeCompressor::Decompress()", "[matrix]") {
  SECTION("Invalid compressed matrix") {
    matrix_compressor::CompressedMatrix compressed;
    compressed.is_valid = false;
    auto bc = matrix_compressor::BlazeCompressor();
    REQUIRE_THROWS_AS(bc.Decompress(compressed), std::invalid_argument);
  }
  SECTION("Random matrix") {
    DataGenerator generator;
    auto matrix = generator.GenerateSparseMatrix(100, 100, 0.1);
    auto compressed = matrix_compressor::BlazeCompressor().Compress(matrix);

    auto decompressed =
        matrix_compressor::BlazeCompressor().Decompress(compressed);
    REQUIRE(decompressed.rows() * decompressed.columns() > 0);
  }
}

TEST_CASE("Decompressed matrix must be equal to origin", "[matrix]") {
  DataGenerator generator;

  /* Define matrix parameters */
  auto rows = GENERATE(10, 100, 5000);
  auto columns = GENERATE(10, 100, 5000);
  auto ratio = GENERATE(0.0, 0.1, 0.2, 0.3, 0.4, 0.6, 0.7, 0.8, 0.9, 1.0);

  /* Generate matrix */
  auto matrix = generator.GenerateSparseMatrix(rows, columns, ratio);

  /* Compress */
  auto compressed = matrix_compressor::BlazeCompressor().Compress(matrix);
  REQUIRE(compressed.is_valid);

  /* Decompress */
  blaze::CompressedMatrix<float> decompressed =
      matrix_compressor::BlazeCompressor().Decompress(compressed);

  REQUIRE(matrix == decompressed);
}
