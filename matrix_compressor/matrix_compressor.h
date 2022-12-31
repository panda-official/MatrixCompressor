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
  /**
   * Compress indexes
   * @param indexes input integers
   * @param compressed output encoded data
   * @return compressed data size in bytes
   */
  size_t CompressIndexes(const std::vector<uint32_t>& indexes,
                         std::vector<uint8_t>& compressed);

  /**
   * Decompress indexes
   * @param compressed input encoded data
   * @param indexes output integers which must has length equal to original data
   * return read data size
   */
  size_t DecompressIndexes(const std::vector<uint8_t>& compressed,
                           std::vector<uint32_t>& indexes);

  /**
   * Compress values
   * @param values unput floats
   * @param compressed output encoded data
   * @return compressed data size in bytes
   */
  size_t CompressValues(const std::vector<float>& values,
                        std::vector<uint8_t>& compressed);

  /**
   * Decompress values
   * @param compressed input encoded data
   * @param values output floats which must has length equal to original data
   * return read data size
   */
  size_t DecompressValues(const std::vector<uint8_t>& compressed,
                          std::vector<float>& values);
};
}  // namespace matrix_compressor
