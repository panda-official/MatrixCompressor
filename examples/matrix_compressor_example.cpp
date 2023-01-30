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

  blaze::DynamicMatrix<float> matrix = blaze::zero<float>(rows, cols);
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
  std::cout << "Compresed data size relative to CSR, in bytes:" << std::endl;
  std::cout << data.rows.size() << "/"
            << (data.rows_number + 1) * sizeof(uint32_t) << std::endl;
  std::cout << data.columns.size() << "/" << data.nonzero * sizeof(uint32_t)
            << std::endl;
  std::cout << data.values.size() << "/" << data.nonzero * sizeof(float)
            << std::endl;

  size_t compressed_size =
      data.rows.size() + data.columns.size() + data.values.size();
  size_t csr_size = (data.nonzero + data.rows_number + 1) * sizeof(uint32_t) +
                    data.nonzero * sizeof(float);

  std::cout << "Total: " << compressed_size << "/" << csr_size << ", "
            << 100 * compressed_size / csr_size << "%" << std::endl;

  return EXIT_SUCCESS;
}
