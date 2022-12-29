#include "matrix_compressor/matrix_compressor.h"

#include <fpzip.h>
#include <streamvbyte.h>

#include <iostream>

namespace matrix_compressor {

CompressedVector compress(const blaze::CompressedVector<float>& vector) {
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

  /* Compress vector indexes */
  std::vector<uint8_t> compressed;
  compressed.resize(streamvbyte_max_compressedbytes(indexes.size()));
  size_t compressed_size =
      streamvbyte_encode(indexes.data(), indexes.size(), compressed.data()) +
      STREAMVBYTE_PADDING;
  compressed.resize(compressed_size);

  std::cout << "Compressed " << indexes.size() << " integers down to "
            << compressed_size << " bytes." << std::endl;

  /* Compress vector values */
  size_t N = values.size();
  size_t buffer_size = sizeof(float) * N + 1024;
  std::vector<uint8_t> compressed_values(buffer_size);
  FPZ* fpz =
      fpzip_write_to_buffer(reinterpret_cast<void*>(compressed_values.data()),
                            compressed_values.size());

  fpz->type = FPZIP_TYPE_FLOAT;
  fpz->prec = 0;
  fpz->nx = N;
  fpz->ny = 1;
  fpz->nz = 1;
  fpz->nf = 1;
  size_t hs = fpzip_write_header(fpz);
  size_t ds = fpzip_write(fpz, reinterpret_cast<void*>(values.data()));
  fpzip_write_close(fpz);
  std::cout << "Compressed " << N << " floats down to " << hs + ds << " bytes."
            << std::endl;

  return {indexes.size(), vector.size(), compressed, compressed_values};
}

blaze::CompressedVector<float> decompress(CompressedVector& compressed) {
  /* Decompress indexes */
  std::vector<uint32_t> indexes;
  indexes.resize(compressed.nonzero);
  streamvbyte_decode(compressed.indexes.data(), indexes.data(),
                     compressed.nonzero);

  /* Decompress values */
  std::vector<float> values;
  values.resize(compressed.size);
  FPZ* fpz =
      fpzip_read_from_buffer(reinterpret_cast<void*>(compressed.values.data()));
  fpzip_read_header(fpz);
  fpzip_read(fpz, reinterpret_cast<void*>(values.data()));
  fpzip_read_close(fpz);

  /* Create vector */
  blaze::CompressedVector<float> vector(compressed.size);
  for (size_t i = 0; i < compressed.nonzero; ++i) {
    vector[indexes[i]] = values[i];
  }

  return vector;
}
}  // namespace matrix_compressor
