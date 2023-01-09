// Copyright 2020-2021 PANDA GmbH

#include <blaze/Blaze.h>
#include <matrix_compressor/matrix_compressor.h>

#include <iostream>
#include <random>

int main(int argc, char* argv[]) {
  if (argc < 4) {
    std::cout << "Usage: " << argv[0] << " <rows> <cols> <ratio>" << std::endl;
    return 1;
  }

  size_t rows = atol(argv[1]);
  size_t cols = atol(argv[2]);
  float ratio = atof(argv[3]);

  std::default_random_engine random_engine_;
  std::uniform_real_distribution<float> distribution_{0, 1};

  blaze::CompressedMatrix<float> matrix(rows, cols);
  for (size_t i = 0; i < rows; ++i) {
    for (size_t j = 0; j < cols; ++j) {
      if (distribution_(random_engine_) < ratio) {
        matrix(i, j) = distribution_(random_engine_);
      }
    }
  }

  std::cout << "Start compressing" << std::endl;
  auto data = matrix_compressor::BlazeCompressor().Compress(matrix);

  /* Print the compressed data. */
  std::cout << "Compresed data size:" << std::endl;
  std::cout << data.rows.size() << "/" << data.nonzero * sizeof(uint32_t)
            << std::endl;
  std::cout << data.columns.size() << "/" << data.nonzero * sizeof(uint32_t)
            << std::endl;
  std::cout << data.values.size() << "/" << data.nonzero * sizeof(float)
            << std::endl;
  std::cout << "Total: "
            << data.rows.size() + data.columns.size() + data.values.size()
            << std::endl;

  return 0;
}
