#include <blaze/Blaze.h>
#include <matrix_compressor/matrix_compressor.h>

#include <iostream>
#include <random>

using namespace matrix_compressor;

class DataGenerator {
  std::default_random_engine random_engine_;
  std::uniform_real_distribution<float> distribution_{0, 1};

 public:
  blaze::CompressedVector<float> GenerateSparseVector(size_t i, float ratio);
  blaze::DynamicMatrix<float> GenerateMatrix2d(size_t rows, size_t cols);
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
                                                            size_t cols) {
  return blaze::generate<blaze::rowMajor>(
      rows, cols,
      [this](size_t i, size_t j) { return distribution_(random_engine_); });
}

int main() {
  DataGenerator generator;
  auto vector = generator.GenerateSparseVector(100, 0.1);

  compress(vector);

  return 0;
}
