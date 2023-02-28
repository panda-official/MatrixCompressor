// Copyright 2020-2022 PANDA GmbH

#include "matrix_compressor/matrix_compressor.h"

#include <fpzip.h>
#include <streamvbyte.h>

#include <fstream>
#include <iostream>

namespace matrix_compressor {

std::tuple<std::vector<uint32_t>, std::vector<float>> ConvertToCSR(
    const blaze::CompressedMatrix<float>& matrix) {
  /* Check input */
  if (matrix.rows() == 0 || matrix.columns() == 0) {
    throw std::invalid_argument("Matrix is empty");
  }

  /* Indexes */
  std::vector<uint32_t> indexes;
  indexes.reserve(matrix.nonZeros());

  /* Values */
  std::vector<float> values;
  values.reserve(matrix.nonZeros());

  /* Fill indexes and value */
  for (size_t row = 0; row < matrix.rows(); ++row) {
    for (auto it = matrix.begin(row); it != matrix.end(row); ++it) {
      indexes.push_back(static_cast<uint32_t>(it->index() * row));
      values.push_back(it->value());
    }
  }

  return {indexes, values};
}

blaze::CompressedMatrix<float> ConvertFromCSR(
    const std::vector<uint32_t>& indexes, const std::vector<float>& values,
    const ArchivedMatrix& compressed) {
  blaze::CompressedMatrix<float> matrix(compressed.rows_number,
                                        compressed.cols_number);
  matrix.reserve(values.size());

  /* Fill */
  for (auto row = 0; row < compressed.rows_number; ++row) {
    matrix.reserve(row, compressed.cols_number);
    for (auto col = 0; col < compressed.cols_number; ++col) {
    }
    matrix.finalize(row);
  }
  return matrix;
}

ArchivedVector BlazeCompressor::Compress(
    const blaze::CompressedVector<float>& vector) {
  /* Check input */
  if (vector.size() == 0) {
    return {};
  }

  if (vector.nonZeros() == 0) {
    return {};
  }

  /* Indexes */
  std::vector<uint32_t> indexes;
  indexes.reserve(vector.nonZeros());

  /* Values */
  std::vector<float> values;
  values.reserve(vector.nonZeros());

  /* Fill indexes and value */
  for (auto it = vector.begin(); it != vector.end(); ++it) {
    indexes.push_back(static_cast<uint32_t>(it->index()));
    values.push_back(it->value());
  }

  /* Compress indexes */
  std::vector<uint8_t> compressed_indexes;
  auto compressed_indexes_size = CompressIndexes(indexes, &compressed_indexes);

  /* Compress values */
  std::vector<uint8_t> compressed_values;
  auto compressed_values_size = CompressValues(values, &compressed_values);

  return {true, indexes.size(), vector.size(), compressed_indexes,
          compressed_values};
}

blaze::CompressedVector<float> BlazeCompressor::Decompress(
    const ArchivedVector& compressed) {
  if (!compressed.is_valid) {
    return {};
  }

  /* Decompress indexes */
  std::vector<uint32_t> indexes;
  indexes.resize(compressed.nonzero);
  DecompressIndexes(compressed.indexes, &indexes);

  /* Decompress values */
  std::vector<float> values;
  values.resize(compressed.nonzero);
  DecompressValues(compressed.values, &values);

  /* Create vector */
  blaze::CompressedVector<float> vector(compressed.size);
  for (size_t i = 0; i < compressed.nonzero; ++i) {
    vector[indexes[i]] = values[i];
  }

  return vector;
}

ArchivedMatrix BlazeCompressor::Compress(
    const blaze::CompressedMatrix<float>& matrix) {
  auto [indexes, values] = ConvertToCSR(matrix);

  ArchivedMatrix archived_matrix{
      true, values.size(), matrix.rows(), matrix.columns(), {}, {}};

  /* Compress indexes */
  CompressIndexes(indexes, &archived_matrix.indexes);

  /* Compress values */
  std::vector<uint8_t> compressed_values;
  CompressValues(values, &archived_matrix.values);

  return archived_matrix;
}

blaze::CompressedMatrix<float> BlazeCompressor::Decompress(
    const ArchivedMatrix& compressed) {
  if (!compressed.is_valid) {
    throw std::invalid_argument("Invalid compressed matrix");
  }

  /* Decompress columns */
  std::vector<uint32_t> indexes;
  indexes.resize(compressed.nonzero);
  DecompressIndexes(compressed.indexes, &indexes);

  /* Decompress values */
  std::vector<float> values;
  values.resize(compressed.nonzero);
  DecompressValues(compressed.values, &values);

  /* Create matrix */
  auto matrix = ConvertFromCSR(indexes, values, compressed);

  return matrix;
}

size_t BlazeCompressor::CompressIndexes(const std::vector<uint32_t>& indexes,
                                        std::vector<uint8_t>* compressed) {
  compressed->resize(streamvbyte_max_compressedbytes(indexes.size()));

  /* Compress */
  size_t compressed_size =
      streamvbyte_encode(indexes.data(), indexes.size(), compressed->data()) +
      STREAMVBYTE_PADDING;

  /* Trim */
  compressed->resize(compressed_size);

  // std::cout << "Compressed " << indexes.size() << " integers down to "
  //           << compressed_size << " bytes." << std::endl;

  return compressed_size;
}

size_t BlazeCompressor::DecompressIndexes(
    const std::vector<uint8_t>& compressed, std::vector<uint32_t>* indexes) {
  streamvbyte_decode(compressed.data(), indexes->data(), indexes->size());
  return 0;
}

size_t BlazeCompressor::CompressValues(const std::vector<float>& values,
                                       std::vector<uint8_t>* compressed) {
  size_t N = values.size();
  size_t buffer_size = sizeof(float) * N + 1024;
  compressed->resize(buffer_size);

  FPZ* fpz = fpzip_write_to_buffer(reinterpret_cast<void*>(compressed->data()),
                                   compressed->size());

  fpz->type = FPZIP_TYPE_FLOAT;
  fpz->prec = 0;
  fpz->nx = N;
  fpz->ny = 1;
  fpz->nz = 1;
  fpz->nf = 1;

  size_t hs = fpzip_write_header(fpz);
  if (hs == 0) {
    fpzip_write_close(fpz);
    throw std::runtime_error(fpzip_errstr[fpzip_errno]);
  }

  size_t ds = fpzip_write(fpz, reinterpret_cast<const void*>(values.data()));
  if (ds == 0) {
    fpzip_write_close(fpz);
    throw std::runtime_error(fpzip_errstr[fpzip_errno]);
  }

  fpzip_write_close(fpz);

  compressed->resize(hs + ds);

  // std::cout << "Compressed " << N << " floats down to " << hs + ds << "
  // bytes."
  //           << std::endl;

  return hs + ds;
}

size_t BlazeCompressor::DecompressValues(const std::vector<uint8_t>& compressed,
                                         std::vector<float>* values) {
  FPZ* fpz =
      fpzip_read_from_buffer(reinterpret_cast<const void*>(compressed.data()));

  if (fpzip_read_header(fpz) == 0) {
    fpzip_read_close(fpz);
    throw std::runtime_error(fpzip_errstr[fpzip_errno]);
  }

  if (fpzip_read(fpz, reinterpret_cast<void*>(values->data())) == 0) {
    fpzip_read_close(fpz);
    throw std::runtime_error(fpzip_errstr[fpzip_errno]);
  }
  fpzip_read_close(fpz);

  return 0;
}

}  // namespace matrix_compressor
