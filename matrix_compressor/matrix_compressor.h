#pragma once

#include <blaze/Blaze.h>

namespace matrix_compressor {

/* Compressed vector data */
struct CompressedVector {
  bool is_valid{false};
  size_t nonzero{0};
  size_t size{0};
  std::vector<uint8_t> indexes;
  std::vector<uint8_t> values;
};

class BlazeCompressor {
 public:
  BlazeCompressor() = default;

  /* Compress a blaze vector */
  CompressedVector Compress(const blaze::CompressedVector<float>& vector);

  /* Decompress a blaze vector */
  blaze::CompressedVector<float> Decompress(
      CompressedVector& compressed_vector);

 private:
  size_t CompressIndexes(const std::vector<uint32_t>& indexes,
                         std::vector<uint8_t>& compressed);
  size_t DecompressIndexes(const std::vector<uint8_t>& compressed,
                           std::vector<uint32_t>& indexes);
  size_t CompressValues(const std::vector<float>& values,
                        std::vector<uint8_t>& compressed);
  size_t DecompressValues(const std::vector<uint8_t>& compressed,
                          std::vector<float>& values);
};
}  // namespace matrix_compressor
