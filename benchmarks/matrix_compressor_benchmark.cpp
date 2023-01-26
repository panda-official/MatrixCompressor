// Copyright 2020-2022 PANDA GmbH

#include <blaze/Blaze.h>
#include <matrix_compressor/matrix_compressor.h>

#include <iostream>
#include <random>

#include <catch2/benchmark/catch_benchmark_all.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

TEST_CASE("BlazeCompressor") {
  std::default_random_engine random_engine_;
  std::uniform_real_distribution<float> distribution_{0, 1};

  size_t rows = 1080;
  size_t cols = 1920;
  auto ratio = GENERATE(0.0, 0.4, 0.8, 1.0);

  blaze::DynamicMatrix<float> matrix = blaze::zero<float>(rows, cols);
  for (size_t i = 0; i < rows; ++i) {
    for (size_t j = 0; j < cols; ++j) {
      if (distribution_(random_engine_) < ratio) {
        matrix(i, j) = distribution_(random_engine_);
      }
    }
  }

  blaze::CompressedMatrix<float> compressed_matrix(matrix);

  BENCHMARK("BlazeCompressor::Compress(), ratio=" + std::to_string(ratio)) {
    auto data =
        matrix_compressor::BlazeCompressor().Compress(compressed_matrix);
    return data;
  };

  auto data = matrix_compressor::BlazeCompressor().Compress(compressed_matrix);
  BENCHMARK("BlazeCompressor::Decompress(), ratio=" + std::to_string(ratio)) {
    auto decompressed_matrix =
        matrix_compressor::BlazeCompressor().Decompress(data);
    return decompressed_matrix;
  };
}
