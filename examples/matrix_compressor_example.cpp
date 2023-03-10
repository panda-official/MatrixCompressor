// Copyright 2020-2022 PANDA GmbH

#include <blaze/Blaze.h>
#include <matrix_compressor/matrix_compressor.h>

#include <iostream>
#include <random>

int main(int argc, char* argv[]) {
  if (argc < 4) {
    std::cout << "Usage: " << argv[0] << " <rows> <cols> <ratio>" << std::endl;
    return EXIT_FAILURE;
  }

  /* Read input */
  size_t rows = atol(argv[1]);
  size_t cols = atol(argv[2]);
  float ratio = atof(argv[3]);

  std::default_random_engine random_engine_;
  std::uniform_real_distribution<float> distribution_{0, 1};

  blaze::CompressedMatrix<float> matrix = blaze::zero<float>(rows, cols);
  for (size_t i = 0; i < rows; ++i) {
    for (size_t j = 0; j < cols; ++j) {
      if (distribution_(random_engine_) < ratio) {
        matrix(i, j) = distribution_(random_engine_);
      }
    }
  }

  std::cout << "Start compressing" << std::endl;
  auto data = matrix_compressor::BlazeCompressor().Compress(matrix, 16);

  /* Print the compressed data. */
  std::cout << "Compresed data size relative to CSR, in bytes:" << std::endl;
  std::cout << data.indexes.size() << "/" << data.nonzero * sizeof(uint64_t)
            << std::endl;
  std::cout << data.values.size() << "/" << data.nonzero * sizeof(float)
            << std::endl;

  std::stringstream ss;
  blaze::Archive archive(ss);
  archive << matrix;

  auto blaze_matrix_size = static_cast<double>(ss.str().size());
  auto compressed_size = static_cast<double>(
      data.indexes.size() + data.values.size());

  std::cout << "Blaze matrix size, in bytes:" << ss.str().size() << std::endl;
  std::cout << "Compressed size, in bytes:" << compressed_size << std::endl;
  std::cout << "Compression ratio:" <<  compressed_size / blaze_matrix_size
            << std::endl;
  return EXIT_SUCCESS;
}
