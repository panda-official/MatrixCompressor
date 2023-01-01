#pragma once

#include <blaze/Blaze.h>

namespace matrix_compressor {

/* Compressed vector data */
struct CompressedVector {
  bool is_valid{false};         /**< state */
  size_t nonzero{0};            /**< number of non-zero elements */
  size_t size{0};               /**< vector size */
  std::vector<uint8_t> indexes; /**< encoded indexes */
  std::vector<uint8_t> values;  /**< encoded values */
};

/* Compressed matrix data */
struct CompressedMatrix {
  bool is_valid{false};         /**< state */
  size_t nonzero{0};            /**< number of non-zero elements */
  size_t cols{0};               /**< matrix columns */
  std::vector<uint8_t> columns; /**< encoded column indexes */
  std::vector<uint8_t> rows;    /**< encoded row indexes */
  std::vector<uint8_t> values;  /**< encoded values */
};

class BlazeCompressor {
 public:
  BlazeCompressor() = default;

  /**
   * Compress a blaze::CompressedVector<float>
   * @param vector
   * @return compressed data
   */
  CompressedVector Compress(const blaze::CompressedVector<float>& vector);

  /**
   * Decompress a blaze::CompressedVector<float>
   * @param compressed compressed data
   * @return decompressed vector
   */
  blaze::CompressedVector<float> Decompress(
      CompressedVector& compressed_vector);

  /**
   * Compress a blaze::CompressedMatrix<float>
   * @param matrix
   * @return compressed data
   */
  CompressedMatrix Compress(const blaze::CompressedMatrix<float>& matrix);

 private:
  /**
   * Convert sparse matrix to CSR representation
   * @param matrix
   * @return CSR representation
   */
  std::tuple<std::vector<uint32_t>, std::vector<uint32_t>, std::vector<float>>
  ConvertToCSR(const blaze::CompressedMatrix<float>& matrix);

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
