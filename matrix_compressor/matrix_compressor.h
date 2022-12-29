#pragma once

#include <blaze/Blaze.h>

namespace matrix_compressor {

/* Compressed vector data */
struct CompressedVector {
  size_t nonzero{0};
  size_t size{0};
  std::vector<uint8_t> indexes;
  std::vector<uint8_t> values;
};

CompressedVector compress(const blaze::CompressedVector<float>& vector);
blaze::CompressedVector<float> decompress(CompressedVector& compressed);

}  // namespace matrix_compressor
